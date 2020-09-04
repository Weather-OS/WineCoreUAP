/*
 * 2020 Jacek Caban for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "wine/test.h"

#include <windows.h>

static HRESULT (WINAPI *pCreatePseudoConsole)(COORD,HANDLE,HANDLE,DWORD,HPCON*);
static void (WINAPI *pClosePseudoConsole)(HPCON);

static char console_output[4096];
static unsigned int console_output_count;
static HANDLE console_pipe;
static HANDLE child_pipe;

#define fetch_console_output() fetch_console_output_(__LINE__)
static void fetch_console_output_(unsigned int line)
{
    OVERLAPPED o;
    DWORD count;
    BOOL ret;

    if (console_output_count == sizeof(console_output)) return;

    o.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    ret = ReadFile(console_pipe, console_output + console_output_count,
                   sizeof(console_output) - console_output_count, NULL, &o);
    if (!ret)
    {
        ok_(__FILE__,line)(GetLastError() == ERROR_IO_PENDING, "read failed: %u\n", GetLastError());
        if (GetLastError() != ERROR_IO_PENDING) return;
        WaitForSingleObject(o.hEvent, 1000);
    }
    ret = GetOverlappedResult(console_pipe, &o, &count, FALSE);
    if (!ret && GetLastError() == ERROR_IO_PENDING)
        CancelIoEx(console_pipe, &o);

    ok_(__FILE__,line)(ret, "Read file failed: %u\n", GetLastError());
    CloseHandle(o.hEvent);
    if (ret) console_output_count += count;
}

#define expect_empty_output() expect_empty_output_(__LINE__)
static void expect_empty_output_(unsigned int line)
{
    DWORD avail;
    BOOL ret;

    ret = PeekNamedPipe(console_pipe, NULL, 0, NULL, &avail, NULL);
    ok_(__FILE__,line)(ret, "PeekNamedPipe failed: %u\n", GetLastError());
    ok_(__FILE__,line)(!avail, "avail = %u\n", avail);
    if (avail) fetch_console_output_(line);
    ok_(__FILE__,line)(!console_output_count, "expected empty buffer, got %s\n",
                       wine_dbgstr_an(console_output, console_output_count));
    console_output_count = 0;
}

#define expect_output_sequence(a) expect_output_sequence_(__LINE__,0,a)
#define expect_output_sequence_ctx(a,b) expect_output_sequence_(__LINE__,a,b)
static void expect_output_sequence_(unsigned int line, unsigned ctx, const char *expect)
{
    size_t len = strlen(expect);
    if (console_output_count < len) fetch_console_output_(line);
    if (len <= console_output_count && !memcmp(console_output, expect, len))
    {
        console_output_count -= len;
        memmove(console_output, console_output + len, console_output_count);
    }
    else ok_(__FILE__,line)(0, "%x: expected %s got %s\n", ctx, wine_dbgstr_a(expect),
                            wine_dbgstr_an(console_output, console_output_count));
}

#define skip_sequence(a) skip_sequence_(__LINE__,a)
static BOOL skip_sequence_(unsigned int line, const char *expect)
{
    size_t len = strlen(expect);
    DWORD avail;
    BOOL r;

    r = PeekNamedPipe(console_pipe, NULL, 0, NULL, &avail, NULL);
    if (!console_output_count && r && !avail)
    {
        Sleep(50);
        r = PeekNamedPipe(console_pipe, NULL, 0, NULL, &avail, NULL);
    }
    if (r && avail) fetch_console_output_(line);

    if (len > console_output_count || memcmp(console_output, expect, len)) return FALSE;
    console_output_count -= len;
    memmove(console_output, console_output + len, console_output_count);
    return TRUE;
}

#define skip_byte(a) skip_byte_(__LINE__,a)
static BOOL skip_byte_(unsigned int line, char ch)
{
    if (!console_output_count || console_output[0] != ch) return FALSE;
    console_output_count--;
    memmove(console_output, console_output + 1, console_output_count);
    return TRUE;
}

#define expect_hide_cursor() expect_hide_cursor_(__LINE__)
static void expect_hide_cursor_(unsigned int line)
{
    if (!console_output_count) fetch_console_output_(line);
    ok_(__FILE__,line)(skip_sequence_(line, "\x1b[25l") || broken(skip_sequence_(line, "\x1b[?25l")),
                       "expected hide cursor escape\n");
}

#define skip_hide_cursor() skip_hide_cursor_(__LINE__)
static BOOL skip_hide_cursor_(unsigned int line)
{
    if (!console_output_count) fetch_console_output_(line);
    return skip_sequence_(line, "\x1b[25l") || broken(skip_sequence_(line, "\x1b[?25l"));
}

#define expect_erase_line(a) expect_erase_line_(__LINE__,a)
static BOOL expect_erase_line_(unsigned line, unsigned int cnt)
{
    char buf[16];
    if (skip_sequence("\x1b[K")) return FALSE;
    ok(broken(1), "expected erase line\n");
    sprintf(buf, "\x1b[%uX", cnt);
    expect_output_sequence(buf);  /* erase the rest of the line */
    sprintf(buf, "\x1b[%uC", cnt);
    expect_output_sequence(buf);  /* move cursor to the end of the line */
    return TRUE;
}

enum req_type
{
    REQ_SET_CURSOR,
    REQ_SET_TITLE,
    REQ_WRITE_CHARACTERS,
    REQ_WRITE_OUTPUT,
};

struct pseudoconsole_req
{
    enum req_type type;
    union
    {
        WCHAR string[1];
        COORD coord;
        struct
        {
            COORD coord;
            unsigned int len;
            WCHAR buf[1];
        } write_characters;
        struct
        {
            COORD size;
            COORD coord;
            SMALL_RECT region;
            CHAR_INFO buf[1];
        } write_output;
    } u;
};

static void child_string_request(enum req_type type, const WCHAR *title)
{
    char buf[4096];
    struct pseudoconsole_req *req = (void *)buf;
    size_t len = lstrlenW(title) + 1;
    DWORD count;
    BOOL ret;

    req->type = type;
    memcpy(req->u.string, title, len * sizeof(WCHAR));
    ret = WriteFile(child_pipe, req, FIELD_OFFSET(struct pseudoconsole_req, u.string[len]),
                    &count, NULL);
    ok(ret, "WriteFile failed: %u\n", GetLastError());
}

static void child_write_characters(const WCHAR *buf, unsigned int x, unsigned int y)
{
    char req_buf[4096];
    struct pseudoconsole_req *req = (void *)req_buf;
    size_t len = lstrlenW(buf);
    DWORD count;
    BOOL ret;

    req->type = REQ_WRITE_CHARACTERS;
    req->u.write_characters.coord.X = x;
    req->u.write_characters.coord.Y = y;
    req->u.write_characters.len = len;
    memcpy(req->u.write_characters.buf, buf, len * sizeof(WCHAR));
    ret = WriteFile(child_pipe, req, FIELD_OFFSET(struct pseudoconsole_req, u.write_characters.buf[len + 1]),
                    &count, NULL);
    ok(ret, "WriteFile failed: %u\n", GetLastError());
}

static void child_set_cursor(const unsigned int x, unsigned int y)
{
    struct pseudoconsole_req req;
    DWORD count;
    BOOL ret;

    req.type = REQ_SET_CURSOR;
    req.u.coord.X = x;
    req.u.coord.Y = y;
    ret = WriteFile(child_pipe, &req, sizeof(req), &count, NULL);
    ok(ret, "WriteFile failed: %u\n", GetLastError());
}

#define child_write_output(a,b,c,d,e,f,g,h,j,k,l,m,n) child_write_output_(__LINE__,a,b,c,d,e,f,g,h,j,k,l,m,n)
static void child_write_output_(unsigned int line, CHAR_INFO *buf, unsigned int size_x, unsigned int size_y,
                                unsigned int coord_x, unsigned int coord_y, unsigned int left,
                                unsigned int top, unsigned int right, unsigned int bottom, unsigned int out_left,
                                unsigned int out_top, unsigned int out_right, unsigned int out_bottom)
{
    char req_buf[4096];
    struct pseudoconsole_req *req = (void *)req_buf;
    SMALL_RECT region;
    DWORD count;
    BOOL ret;

    req->type = REQ_WRITE_OUTPUT;
    req->u.write_output.size.X = size_x;
    req->u.write_output.size.Y = size_y;
    req->u.write_output.coord.X = coord_x;
    req->u.write_output.coord.Y = coord_y;
    req->u.write_output.region.Left   = left;
    req->u.write_output.region.Top    = top;
    req->u.write_output.region.Right  = right;
    req->u.write_output.region.Bottom = bottom;
    memcpy(req->u.write_output.buf, buf, size_x * size_y * sizeof(*buf));
    ret = WriteFile(child_pipe, req, FIELD_OFFSET(struct pseudoconsole_req, u.write_output.buf[size_x * size_y]), &count, NULL);
    ok_(__FILE__,line)(ret, "WriteFile failed: %u\n", GetLastError());
    ret = ReadFile(child_pipe, &region, sizeof(region), &count, NULL);
    ok_(__FILE__,line)(ret, "WriteFile failed: %u\n", GetLastError());
    ok_(__FILE__,line)(region.Left == out_left, "Left = %u\n", region.Left);
    ok_(__FILE__,line)(region.Top == out_top, "Top = %u\n", region.Top);
    ok_(__FILE__,line)(region.Right == out_right, "Right = %u\n", region.Right);
    ok_(__FILE__,line)(region.Bottom == out_bottom, "Bottom = %u\n", region.Bottom);
}

static void test_tty_output(void)
{
    CHAR_INFO char_info_buf[2048], char_info;
    unsigned int i;

    /* simple write chars */
    child_write_characters(L"child", 3, 4);
    expect_hide_cursor();
    expect_output_sequence("\x1b[5;4H");   /* set cursor */
    expect_output_sequence("child");
    expect_output_sequence("\x1b[H");      /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    /* wrapped write chars */
    child_write_characters(L"bound", 28, 6);
    expect_hide_cursor();
    expect_output_sequence("\x1b[7;1H");   /* set cursor */
    expect_output_sequence("                            bo\r\nund");
    expect_erase_line(27);
    expect_output_sequence("\x1b[H");      /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    /* fill line 4 with a few simple writes */
    child_write_characters(L"xxx", 13, 4);
    expect_hide_cursor();
    expect_output_sequence("\x1b[5;14H");  /* set cursor */
    expect_output_sequence("xxx");
    expect_output_sequence("\x1b[H");      /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    /* write one char at the end of row */
    child_write_characters(L"y", 29, 4);
    expect_hide_cursor();
    expect_output_sequence("\x1b[5;30H");  /* set cursor */
    expect_output_sequence("y");
    expect_output_sequence("\x1b[H");      /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    /* wrapped write chars */
    child_write_characters(L"zz", 29, 4);
    expect_hide_cursor();
    expect_output_sequence("\x1b[5;1H");   /* set cursor */
    expect_output_sequence("   child     xxx             z");
    expect_output_sequence("\r\nz");
    expect_erase_line(29);
    expect_output_sequence("\x1b[H");      /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    /* trailing spaces */
    child_write_characters(L"child        ", 3, 4);
    expect_hide_cursor();
    expect_output_sequence("\x1b[5;4H");   /* set cursor */
    expect_output_sequence("child        ");
    expect_output_sequence("\x1b[H");      /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    child_set_cursor(2, 3);
    expect_hide_cursor();
    expect_output_sequence("\x1b[4;3H");   /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    child_string_request(REQ_SET_TITLE, L"new title");
    fetch_console_output();
    skip_sequence("\x1b[?25l");            /* hide cursor */
    expect_output_sequence("\x1b]0;new title\x07"); /* set title */
    skip_sequence("\x1b[?25h");            /* show cursor */
    expect_empty_output();

    for (i = 0; i < ARRAY_SIZE(char_info_buf); i++)
    {
        char_info_buf[i].Char.UnicodeChar = '0' + i % 10;
        char_info_buf[i].Attributes = 0;
    }

    child_write_output(char_info_buf, /* size */ 7, 8, /* coord */ 1, 2,
                       /* region */ 3, 7, 5, 9, /* out region */ 3, 7, 5, 9);
    expect_hide_cursor();
    expect_output_sequence("\x1b[30m");    /* foreground black */
    expect_output_sequence("\x1b[8;4H");   /* set cursor */
    expect_output_sequence("567");
    expect_output_sequence("\x1b[9;4H");   /* set cursor */
    expect_output_sequence("234");
    expect_output_sequence("\x1b[10;4H");  /* set cursor */
    expect_output_sequence("901");
    expect_output_sequence("\x1b[4;3H");   /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    child_write_output(char_info_buf, /* size */ 2, 3, /* coord */ 1, 2,
                       /* region */ 3, 8, 15, 19, /* out region */ 3, 8, 3, 8);
    expect_hide_cursor();
    if (skip_sequence("\x1b[m"))           /* default attr */
        expect_output_sequence("\x1b[30m");/* foreground black */
    expect_output_sequence("\x1b[9;4H");   /* set cursor */
    expect_output_sequence("5");
    expect_output_sequence("\x1b[4;3H");   /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    child_write_output(char_info_buf, /* size */ 3, 4, /* coord */ 1, 2,
                       /* region */ 3, 8, 15, 19, /* out region */ 3, 8, 4, 9);
    expect_hide_cursor();
    if (skip_sequence("\x1b[m"))           /* default attr */
        expect_output_sequence("\x1b[30m");/* foreground black */
    expect_output_sequence("\x1b[9;4H");   /* set cursor */
    expect_output_sequence("78");
    expect_output_sequence("\x1b[10;4H");  /* set cursor */
    expect_output_sequence("01");
    expect_output_sequence("\x1b[4;3H");   /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    child_write_output(char_info_buf, /* size */ 7, 8, /* coord */ 2, 3,
                       /* region */ 28, 38, 31, 60, /* out region */ 28, 38, 29, 39);
    expect_hide_cursor();
    if (skip_sequence("\x1b[m"))           /* default attr */
        expect_output_sequence("\x1b[30m");/* foreground black */
    expect_output_sequence("\x1b[39;29H"); /* set cursor */
    expect_output_sequence("34");
    expect_output_sequence("\x1b[40;29H"); /* set cursor */
    expect_output_sequence("01");
    expect_output_sequence("\x1b[4;3H");   /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    child_write_output(char_info_buf, /* size */ 7, 8, /* coord */ 1, 2,
                       /* region */ 0, 7, 5, 9, /* out region */ 0, 7, 5, 9);
    expect_hide_cursor();
    if (skip_sequence("\x1b[m"))           /* default attr */
        expect_output_sequence("\x1b[30m");/* foreground black */
    expect_output_sequence("\x1b[8;1H");   /* set cursor */
    expect_output_sequence("567890\r\n");
    expect_output_sequence("234567\r\n");
    expect_output_sequence("901234");
    expect_output_sequence("\x1b[4;3H");   /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    child_write_characters(L"xxx", 3, 10);
    expect_hide_cursor();
    expect_output_sequence("\x1b[m");      /* default attributes */
    expect_output_sequence("\x1b[11;4H");  /* set cursor */
    expect_output_sequence("xxx");
    expect_output_sequence("\x1b[4;3H");   /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();

    /* test attributes */
    for (i = 0; i < 0x100 - 0xff; i++)
    {
        unsigned int expect;
        char expect_buf[16];
        char_info.Char.UnicodeChar = 'a';
        char_info.Attributes = i;
        child_write_output(&char_info, /* size */ 1, 1, /* coord */ 0, 0,
                           /* region */ 12, 3, 12, 3, /* out region */ 12, 3, 12, 3);
        expect_hide_cursor();
        if (i != 0x190 && i && ((i & 0xff) != 8)) expect_output_sequence_ctx(i, "\x1b[m");
        if ((i & 0x0f) != 7)
        {
            expect = 30;
            if (i & FOREGROUND_BLUE)  expect += 4;
            if (i & FOREGROUND_GREEN) expect += 2;
            if (i & FOREGROUND_RED)   expect += 1;
            if (i & FOREGROUND_INTENSITY) expect += 60;
            sprintf(expect_buf, "\x1b[%um", expect);
            expect_output_sequence_ctx(i, expect_buf);
        }
        if (i & 0xf0)
        {
            expect = 40;
            if (i & BACKGROUND_BLUE)  expect += 4;
            if (i & BACKGROUND_GREEN) expect += 2;
            if (i & BACKGROUND_RED)   expect += 1;
            if (i & BACKGROUND_INTENSITY) expect += 60;
            sprintf(expect_buf, "\x1b[%um", expect);
            expect_output_sequence_ctx(i, expect_buf);
        }
        if (!skip_sequence("\x1b[10C"))
            expect_output_sequence_ctx(i, "\x1b[4;13H"); /* set cursor */
        expect_output_sequence("a");
        if (!skip_sequence("\x1b[11D"))
            expect_output_sequence("\x1b[4;3H"); /* set cursor */
        expect_output_sequence("\x1b[?25h");     /* show cursor */
        expect_empty_output();
    }

    char_info_buf[0].Attributes = FOREGROUND_GREEN;
    char_info_buf[1].Attributes = FOREGROUND_GREEN | BACKGROUND_RED;
    char_info_buf[2].Attributes = BACKGROUND_RED;
    child_write_output(char_info_buf, /* size */ 7, 8, /* coord */ 0, 0,
                       /* region */ 7, 0, 9, 0, /* out region */ 7, 0, 9, 0);
    expect_hide_cursor();
    skip_sequence("\x1b[m");               /* default attr */
    expect_output_sequence("\x1b[32m");    /* foreground black */
    expect_output_sequence("\x1b[1;8H");   /* set cursor */
    expect_output_sequence("0");
    expect_output_sequence("\x1b[41m");    /* backgorund red */
    expect_output_sequence("1");
    expect_output_sequence("\x1b[30m");    /* foreground black */
    expect_output_sequence("2");
    expect_output_sequence("\x1b[4;3H");   /* set cursor */
    expect_output_sequence("\x1b[?25h");   /* show cursor */
    expect_empty_output();
}

static void child_process(HANDLE pipe)
{
    HANDLE output, input;
    DWORD size, count;
    char buf[4096];
    BOOL ret;

    output = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
    ok(output != INVALID_HANDLE_VALUE, "could not open console output\n");

    input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
    ok(output != INVALID_HANDLE_VALUE, "could not open console output\n");

    while(ReadFile(pipe, buf, sizeof(buf), &size, NULL))
    {
        const struct pseudoconsole_req *req = (void *)buf;
        switch (req->type)
        {
        case REQ_SET_CURSOR:
            ret = SetConsoleCursorPosition(output, req->u.coord);
            ok(ret, "SetConsoleCursorPosition failed: %u\n", GetLastError());
            break;

        case REQ_SET_TITLE:
            ret = SetConsoleTitleW(req->u.string);
            ok(ret, "SetConsoleTitleW failed: %u\n", GetLastError());
            break;

        case REQ_WRITE_CHARACTERS:
            ret = WriteConsoleOutputCharacterW(output, req->u.write_characters.buf,
                                               req->u.write_characters.len,
                                               req->u.write_characters.coord, &count);
            ok(ret, "WriteConsoleOutputCharacterW failed: %u\n", GetLastError());
            break;

        case REQ_WRITE_OUTPUT:
            {
                SMALL_RECT region = req->u.write_output.region;
                ret = WriteConsoleOutputW(output, req->u.write_output.buf, req->u.write_output.size, req->u.write_output.coord, &region);
                ok(ret, "WriteConsoleOutput failed: %u\n", GetLastError());
                ret = WriteFile(pipe, &region, sizeof(region), &count, NULL);
                ok(ret, "WriteFile failed: %u\n", GetLastError());
                break;
            }

        default:
            ok(0, "unexpected request type %u\n", req->type);
        };
    }
    ok(GetLastError() == ERROR_BROKEN_PIPE, "ReadFile failed: %u\n", GetLastError());
    CloseHandle(output);
    CloseHandle(input);
}

static HANDLE run_child(HANDLE console, HANDLE pipe)
{
    STARTUPINFOEXA startup = {{ sizeof(startup) }};
    char **argv, cmdline[MAX_PATH];
    PROCESS_INFORMATION info;
    SIZE_T size;
    BOOL ret;

    InitializeProcThreadAttributeList(NULL, 1, 0, &size);
    startup.lpAttributeList = HeapAlloc(GetProcessHeap(), 0, size);
    InitializeProcThreadAttributeList(startup.lpAttributeList, 1, 0, &size);
    UpdateProcThreadAttribute(startup.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, console,
                              sizeof(console), NULL, NULL);

    winetest_get_mainargs(&argv);
    sprintf(cmdline, "\"%s\" %s child %p", argv[0], argv[1], pipe);
    ret = CreateProcessA(NULL, cmdline, NULL, NULL, TRUE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL,
                         &startup.StartupInfo, &info);
    ok(ret, "CreateProcessW failed: %u\n", GetLastError());

    CloseHandle(info.hThread);
    HeapFree(GetProcessHeap(), 0, startup.lpAttributeList);
    return info.hProcess;
}

static HPCON create_pseudo_console(HANDLE *console_pipe_end, HANDLE *child_process)
{
    SECURITY_ATTRIBUTES sec_attr = { sizeof(sec_attr), NULL, TRUE };
    HANDLE child_pipe_end;
    COORD size = { 30, 40 };
    DWORD read_mode;
    HPCON console;
    HRESULT hres;
    BOOL r;

    console_pipe = CreateNamedPipeW(L"\\\\.\\pipe\\pseudoconsoleconn", PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
                                    PIPE_WAIT | PIPE_TYPE_BYTE, 1, 4096, 4096, NMPWAIT_USE_DEFAULT_WAIT, NULL);
    ok(console_pipe != INVALID_HANDLE_VALUE, "CreateNamedPipeW failed: %u\n", GetLastError());

    *console_pipe_end = CreateFileW(L"\\\\.\\pipe\\pseudoconsoleconn", GENERIC_READ | GENERIC_WRITE,
                                    0, &sec_attr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    ok(*console_pipe_end != INVALID_HANDLE_VALUE, "CreateFile failed: %u\n", GetLastError());

    child_pipe = CreateNamedPipeW(L"\\\\.\\pipe\\pseudoconsoleserver", PIPE_ACCESS_DUPLEX,
                                  PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 1, 5000, 6000,
                                  NMPWAIT_USE_DEFAULT_WAIT, NULL);
    ok(child_pipe != INVALID_HANDLE_VALUE, "CreateNamedPipeW failed: %u\n", GetLastError());

    child_pipe_end = CreateFileW(L"\\\\.\\pipe\\pseudoconsoleserver", GENERIC_READ | GENERIC_WRITE, 0,
                                 &sec_attr, OPEN_EXISTING, 0, NULL);
    ok(child_pipe_end != INVALID_HANDLE_VALUE, "CreateFile failed: %u\n", GetLastError());

    read_mode = PIPE_READMODE_MESSAGE;
    r = SetNamedPipeHandleState(child_pipe_end, &read_mode, NULL, NULL);
    ok(r, "SetNamedPipeHandleState failed: %u\n", GetLastError());

    hres = pCreatePseudoConsole(size, *console_pipe_end, *console_pipe_end, 0, &console);
    ok(hres == S_OK, "CreatePseudoConsole failed: %08x\n", hres);

    *child_process = run_child(console, child_pipe_end);
    CloseHandle(child_pipe_end);
    return console;
}

static void test_pseudoconsole(void)
{
    HANDLE console_pipe_end, child_process;
    BOOL broken_version;
    HPCON console;

    console = create_pseudo_console(&console_pipe_end, &child_process);

    child_string_request(REQ_SET_TITLE, L"test title");
    expect_output_sequence("\x1b[2J");   /* erase display */
    skip_hide_cursor();
    expect_output_sequence("\x1b[m");    /* default attributes */
    expect_output_sequence("\x1b[H");    /* set cursor */
    skip_sequence("\x1b[H");             /* some windows versions emit it twice */
    expect_output_sequence("\x1b]0;test title"); /* set title */
    broken_version = skip_byte(0);       /* some win versions emit nullbyte */
    expect_output_sequence("\x07");
    skip_sequence("\x1b[?25h");          /* show cursor */
    expect_empty_output();

    if (!broken_version) test_tty_output();
    else win_skip("Skipping tty output tests on broken Windows version\n");

    pClosePseudoConsole(console);
    CloseHandle(console_pipe_end);
    CloseHandle(console_pipe);
}

START_TEST(tty)
{
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    char **argv;
    int argc;

    argc = winetest_get_mainargs(&argv);
    if (argc > 3)
    {
        HANDLE pipe;
        sscanf(argv[3], "%p", &pipe);
        child_process(pipe);
        return;
    }

    pCreatePseudoConsole = (void *)GetProcAddress(kernel32, "CreatePseudoConsole");
    pClosePseudoConsole  = (void *)GetProcAddress(kernel32, "ClosePseudoConsole");
    if (!pCreatePseudoConsole)
    {
        win_skip("CreatePseudoConsole is not available\n");
        return;
    }

    test_pseudoconsole();
}
