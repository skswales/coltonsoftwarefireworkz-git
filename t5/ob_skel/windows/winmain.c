/* windows/winmain.c */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/* Copyright (C) 1993-1998 Colton Software Limited
 * Copyright (C) 1998-2015 R W Colton */

#include "common/gflags.h"

#if WINDOWS

#ifndef    __skel_flags_h
#include "ob_skel/flags.h"
#endif

#include "ob_file/xp_file.h"

#include <locale.h> /* for setlocale */

#include <time.h> /* for _tzset */

#if CHECKING
#include <crtdbg.h>
#endif

/*
Windows libraries (saves hacking link properties in project which seem to differ between releases of Visual Studio)
*/

#pragma comment(lib, "delayimp.lib")

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "winspool.lib")

#pragma comment(lib, "msimg32.lib")

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

/*
Request comctl32.dll version 6 for Windows XP visual styles
*/

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

extern
#if defined(__cplusplus)
"C"
#endif
void
gdiplus_startup(void);

extern
#if defined(__cplusplus)
"C"
#endif
void
gdiplus_shutdown(void);

#include "ob_skel/prodinfo.h"

/*
exported global data
*/

/*extern*/ HINSTANCE _g_hInstance;

/*extern*/ HINSTANCE g_hInstancePrev;

/*extern*/ int g_nCmdShow;

/* encoding and decoding of registration info */

static const int /* this is a forever binding */
eor_with_me[16] = { 155, 164, 33, 75, 166, 2, 8, 0, 30, 27, 5, 6, 29, 223, 242, 212 };

/* decode two bytes from c (char stream offset i) to be a character */

_Check_return_
static int
decode_byte(
    _In_reads_bytes_c_(2) PC_BYTE c,
    _In_        int i)
{
    BYTE c1 = *c++; /* MSN */
    BYTE c2 = *c;   /* LSN */
    int k;

    if((c1 >= 'A') && (c1 <= 'F'))
    {
        c1 -= 'A';
        c1 += 10;
    }
    else if((c1 >= CH_DIGIT_ZERO) && (c1 <= CH_DIGIT_NINE))
        c1 -= CH_DIGIT_ZERO;
    else
        return(-1);

    if((c2 >= 'A') && (c2 <= 'F'))
    {
        c2 -= 'A';
        c2 += 10;
    }
    else if((c2 >= CH_DIGIT_ZERO) && (c2 <= CH_DIGIT_NINE))
        c2 -= CH_DIGIT_ZERO;
    else
        return(-1);

    k = ((int) c1 << 4) + (int) c2;

    return(k ^ eor_with_me[i & 0x0F]);
}

/******************************************************************************
*
* copy registration info from registry
*
******************************************************************************/

static void
registry_get_user_name(void)
{
    HKEY hkey;
    DWORD err;

    if(ERROR_SUCCESS == (err = RegOpenKeyExW(HKEY_LOCAL_MACHINE, key_program_wstr, 0, KEY_READ, &hkey)))
    {
        BYTE b[REG_NAME_LENGTH*2 + 1];
        DWORD bl = sizeof32(b);
        DWORD dwType;

        if(ERROR_SUCCESS == (err = RegQueryValueEx(hkey, TEXT("Usr1"), NULL, &dwType, b, &bl)))
        {
            PCTSTR tstr_user_name = (PCTSTR) b;
            tstr_xstrkpy(__user_name, elemof32(__user_name), tstr_user_name);
            trace_1(0, TEXT("__user_name read: %s"), __user_name);
        }

        RegCloseKey(hkey);
    }
}

static void
registry_get_organization_name(void)
{
    HKEY hkey;
    DWORD err;

    if(ERROR_SUCCESS == (err = RegOpenKeyExW(HKEY_LOCAL_MACHINE, key_program_wstr, 0, KEY_READ, &hkey)))
    {
        BYTE b[REG_NAME_LENGTH*2 + 1];
        DWORD bl = sizeof32(b);
        DWORD dwType;

        if(ERROR_SUCCESS == RegQueryValueEx(hkey, TEXT("Usr2"), NULL, &dwType, b, &bl))
        {
            PCTSTR tstr_organ_name = (PCTSTR) b;
            tstr_xstrkpy(__organisation_name, elemof32(__organisation_name), tstr_organ_name);
            trace_1(0, TEXT("__organisation_name read: %s"), __organisation_name);
        }

        RegCloseKey(hkey);
    }
}

/******************************************************************************
*
* copy registration number from registry and
* check that registration number is valid
*
******************************************************************************/

static void
registry_get_registration_number(void)
{
    HKEY hkey;
    DWORD err;

    if(ERROR_SUCCESS == (err = RegOpenKeyExW(HKEY_LOCAL_MACHINE, key_program_wstr, 0, KEY_READ, &hkey)))
    {
        BYTE b[REG_NUMB_LENGTH*2 + 1];
        DWORD bl = sizeof32(b);
        DWORD dwType;

        if(ERROR_SUCCESS == (err = RegQueryValueEx(hkey, TEXT("RegistrationNumber"), NULL, &dwType, b, &bl)))
        {
            /* decode in place */
            BOOL res = TRUE;
            int i = 0;
            PTCH to = __registration_number;
            do  {
                int c = decode_byte(&b[i * sizeof32(TCHAR)], i >> 1);
                if(c < 0)
                {
                    res = FALSE;
                    break;
                }
                *to++ = (TCHAR) c;
                i += 2;
            }
            while(i < REG_NUMB_LENGTH*2);
            *to++ = CH_NULL; /* ensure termination */

            if(res)
            {
                trace_1(0, TEXT("__registration_number decrypted: %s"), report_tstr(__registration_number));
            }
        }

        RegCloseKey(hkey);
    }
}

static void
get_user_info(void)
{
    registry_get_user_name();
    registry_get_organization_name();
    registry_get_registration_number();
}

_Check_return_
static U32
HklmGetProfileString(
    _In_z_      PCTSTR ptzKey,
    _In_z_      PCTSTR ptzDefault,
    _Out_writes_z_(cchReturnBuffer) PTSTR ptzReturnBuffer,
    _InVal_     U32 cchReturnBuffer)
{
    BOOL value_read = FALSE;
    HKEY hkey;
    DWORD err;

    PTR_ASSERT(ptzKey); /* no general wildcard lookup allowed */
    PTR_ASSERT(ptzDefault); /* must supply a default */

    if(ERROR_SUCCESS == (err = RegOpenKeyExW(HKEY_LOCAL_MACHINE, key_program_wstr, 0, KEY_READ, &hkey)))
    {
        DWORD bl = cchReturnBuffer;
        DWORD dwType;

        if(ERROR_SUCCESS == (err = RegQueryValueEx(hkey, ptzKey, NULL, &dwType, (LPBYTE) ptzReturnBuffer, &bl)))
        {
            value_read = TRUE;
        }

        RegCloseKey(hkey);

        if(value_read)
        {
            return(bl - 1);
        }
    }

    /* copy default by hand */
    tstr_xstrkpy(ptzReturnBuffer, cchReturnBuffer, ptzDefault);

    return(0);
}

/* look in registry rather than in a .ini file */

_Check_return_
extern U32
MyGetProfileString(
    _In_z_      PCTSTR ptzKey,
    _In_z_      PCTSTR ptzDefault,
    _Out_writes_z_(cchReturnBuffer) PTSTR ptzReturnBuffer,
    _InVal_     U32 cchReturnBuffer)
{
    BOOL value_read = FALSE;
    HKEY hkey;
    DWORD err;

    PTR_ASSERT(ptzKey); /* no general wildcard lookup allowed */
    PTR_ASSERT(ptzDefault); /* must supply a default */

    if(ERROR_SUCCESS == (err = RegOpenKeyExW(HKEY_CURRENT_USER, key_program_wstr, 0, KEY_READ, &hkey)))
    {
        DWORD bl = cchReturnBuffer;
        DWORD dwType;

        if(ERROR_SUCCESS == (err = RegQueryValueEx(hkey, ptzKey, NULL, &dwType, (LPBYTE) ptzReturnBuffer, &bl)))
        {
            value_read = TRUE;
        }

        RegCloseKey(hkey);

        if(value_read)
        {
            return(bl - 1);
        }
    }

    /* if we failed to look up under current user, look for machine-wide default */
    return(HklmGetProfileString(ptzKey, ptzDefault, ptzReturnBuffer, cchReturnBuffer));
}

static void 
ensure_key_in_current_user(
    HKEY hkey, /* not HKEY_CURRENT_USER but our subkey */
    _In_z_      PCTSTR key_name)
{
    TCHARZ value[BUF_MAX_PATHSTRING];

    if(0 == HklmGetProfileString(key_name, tstr_empty_string, value, elemof32(value)))
        (void) RegSetValueEx(hkey, key_name, 0, REG_SZ, (const BYTE *) value, (DWORD) ((tstrlen32p1(value) /*CH_NULL*/)) * sizeof32(TCHAR));
}

_Check_return_
static BOOL
GetUserAppDataDirectoryName_Fireworkz(
    _Out_writes_z_(elemof_buffer) PTSTR p_dir_name /*filled*/,
    _InVal_     U32 elemof_buffer,
    _InVal_     BOOL create_path_elements)
{
    if(!GetUserAppDataDirectoryName(p_dir_name, elemof_buffer))
        return(FALSE);

    tstr_xstrkat(p_dir_name, elemof_buffer, FILE_DIR_SEP_TSTR TEXT("Colton Software"));
    if(create_path_elements)
        status_consume(file_create_directory(p_dir_name));

    tstr_xstrkat(p_dir_name, elemof_buffer, FILE_DIR_SEP_TSTR TEXT("Fireworkz"));
    if(create_path_elements)
        status_consume(file_create_directory(p_dir_name));

    return(TRUE);
}

static void
copy_this_template_to_current_user_ThisAppData(
    _In_z_  PCTSTR this_template,
    _In_z_  PCTSTR subdirectory)
{
    TCHARZ source_directory[BUF_MAX_PATHSTRING];
    TCHARZ destination_directory[BUF_MAX_PATHSTRING];

    if( (0 != MyGetProfileString(TEXT("Directory"), tstr_empty_string, source_directory, elemof32(source_directory))) &&
        GetUserAppDataDirectoryName_Fireworkz(destination_directory, elemof32(destination_directory), TRUE) )
    {
        TCHARZ source_filename[BUF_MAX_PATHSTRING];

        tstr_xstrkat(
            source_directory, elemof32(source_directory),
            FILE_DIR_SEP_TSTR TEXT("DefaultUser")
            FILE_DIR_SEP_TSTR TEXT("AppData")
            FILE_DIR_SEP_TSTR );

        tstr_xstrkat(source_directory, elemof32(source_directory), subdirectory);

        tstr_xstrkat(destination_directory, elemof32(destination_directory), FILE_DIR_SEP_TSTR);
        tstr_xstrkat(destination_directory, elemof32(destination_directory), subdirectory);

        tstr_xstrkpy(source_filename, elemof32(source_filename), source_directory);
        tstr_xstrkat(source_filename, elemof32(source_filename), FILE_DIR_SEP_TSTR);
        tstr_xstrkat(source_filename, elemof32(source_filename), this_template);

        if(file_is_file(source_filename))
        {
#if 1
            {
            TCHARZ tmp[200 + (BUF_MAX_PATHSTRING * 2)];
            tstr_xstrkpy(tmp, elemof32(tmp), TEXT("Copying "));
            tstr_xstrkat(tmp, elemof32(tmp), this_template);
            tstr_xstrkat(tmp, elemof32(tmp), TEXT("\nfrom "));
            tstr_xstrkat(tmp, elemof32(tmp), source_directory);
            tstr_xstrkat(tmp, elemof32(tmp), TEXT("\nto "));
            tstr_xstrkat(tmp, elemof32(tmp), destination_directory);
            consume_int(MessageBox(NULL, tmp, product_ui_id(), MB_OK | MB_ICONINFORMATION));
            } /*block*/
#endif

            status_consume(file_create_directory(destination_directory));

            tstr_xstrkat(destination_directory, elemof32(destination_directory), FILE_DIR_SEP_TSTR);

            /* Need double CH_NULL termination for SHFileOperation */
            source_filename[tstrlen32(source_filename) + 1] = CH_NULL;
            destination_directory[tstrlen32(destination_directory) + 1] = CH_NULL;

            {
            SHFILEOPSTRUCT shfileop;
            zero_struct(shfileop);
            shfileop.wFunc = FO_COPY;
            shfileop.pFrom = source_filename;
            shfileop.pTo = destination_directory;
            shfileop.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCOPYSECURITYATTRIBS;
            (void) SHFileOperation(&shfileop);
            } /*block*/
        }
    }
}

static void
copy_template_list_to_current_user_ThisAppData(
    _In_z_  PTSTR template_list,
    _In_z_  PCTSTR subdirectory)
{
    PTSTR this_template = template_list;

    for(;;)
    {
        PTSTR end_tstr;

        /* skip numeric element */
        if(NULL == (end_tstr = tstrchr(this_template, CH_COLON)))
            return; /* malformed list */

        this_template = end_tstr + 1; /* skip delimiter */

        end_tstr = tstrchr(this_template, CH_SEMICOLON);

        if(NULL != end_tstr)
            *end_tstr++ = CH_NULL; /* overwrite delimiter */

        copy_this_template_to_current_user_ThisAppData(this_template, subdirectory);

        if(NULL == end_tstr)
            break;

        this_template = end_tstr;
    }
}

static void
delete_this_file_from_current_user_ThisAppData(
    _In_z_  PCTSTR this_file,
    _In_z_  PCTSTR subdirectory)
{
    TCHARZ user_directory[BUF_MAX_PATHSTRING];

    if(GetUserAppDataDirectoryName_Fireworkz(user_directory, elemof32(user_directory), FALSE))
    {
        TCHARZ user_filename[BUF_MAX_PATHSTRING];

        tstr_xstrkat(user_directory, elemof32(user_directory), FILE_DIR_SEP_TSTR);
        tstr_xstrkat(user_directory, elemof32(user_directory), subdirectory);

        tstr_xstrkpy(user_filename, elemof32(user_filename), user_directory);
        tstr_xstrkat(user_filename, elemof32(user_filename), FILE_DIR_SEP_TSTR);
        tstr_xstrkat(user_filename, elemof32(user_filename), this_file);

        if(file_is_file(user_filename))
        {
#if 1
            {
            TCHARZ tmp[200 + (BUF_MAX_PATHSTRING * 2)];
            tstr_xstrkpy(tmp, elemof32(tmp), TEXT("Deleting "));
            tstr_xstrkat(tmp, elemof32(tmp), this_file);
            tstr_xstrkat(tmp, elemof32(tmp), TEXT("\nfrom "));
            tstr_xstrkat(tmp, elemof32(tmp), user_directory);
            consume_int(MessageBox(NULL, tmp, product_ui_id(), MB_OK | MB_ICONINFORMATION));
            } /*block*/
#endif

            status_consume(file_remove(user_filename));
        }
    }
}

static void
delete_template_list_from_current_user_ThisAppData(
    _In_z_  PTSTR template_list,
    _In_z_  PCTSTR subdirectory)
{
    PTSTR this_template = template_list;

    for(;;)
    {
        PTSTR end_tstr;

        /* skip numeric element */
        if(NULL == (end_tstr = tstrchr(this_template, CH_COLON)))
            return; /* malformed list */

        this_template = end_tstr + 1; /* skip delimiter */

        end_tstr = tstrchr(this_template, CH_SEMICOLON);

        if(NULL != end_tstr)
            *end_tstr++ = CH_NULL; /* overwrite delimiter */

        delete_this_file_from_current_user_ThisAppData(this_template, subdirectory);

        if(NULL == end_tstr)
            break;

        this_template = end_tstr;
    }
}

static void
upgrade_ThisAppData(
    _In_z_  PTSTR current_user_template_list,
    _In_z_  PTSTR source_template_list,
    _In_z_  PCTSTR subdirectory)
{
    delete_template_list_from_current_user_ThisAppData(current_user_template_list, subdirectory);

    copy_template_list_to_current_user_ThisAppData(source_template_list, subdirectory);
}

static void
copy_this_template_to_current_user_Templates(
    _In_z_  PCTSTR this_template)
{
    /* Copy the given template file from the DefaultUser Templates directory to the current user's Templates directory */
    TCHARZ source_directory[BUF_MAX_PATHSTRING];
    TCHARZ destination_directory[BUF_MAX_PATHSTRING];

    if( (0 != MyGetProfileString(TEXT("Directory"), tstr_empty_string, source_directory, elemof32(source_directory))) &&
        GetUserTemplatesDirectoryName(destination_directory, elemof32(destination_directory)) )
    {
        TCHARZ source_filename[BUF_MAX_PATHSTRING];

        tstr_xstrkat(
            source_directory, elemof32(source_directory),
            FILE_DIR_SEP_TSTR TEXT("DefaultUser")
            FILE_DIR_SEP_TSTR TEXT("Templates"));

        tstr_xstrkpy(source_filename, elemof32(source_filename), source_directory);
        tstr_xstrkat(source_filename, elemof32(source_filename), FILE_DIR_SEP_TSTR);
        tstr_xstrkat(source_filename, elemof32(source_filename), this_template);

        if(file_is_file(source_filename))
        {
#if 1
            {
            TCHARZ tmp[200 + (BUF_MAX_PATHSTRING * 2)];
            tstr_xstrkpy(tmp, elemof32(tmp), TEXT("Copying "));
            tstr_xstrkat(tmp, elemof32(tmp), this_template);
            tstr_xstrkat(tmp, elemof32(tmp), TEXT("\nfrom "));
            tstr_xstrkat(tmp, elemof32(tmp), source_directory);
            tstr_xstrkat(tmp, elemof32(tmp), TEXT("\nto "));
            tstr_xstrkat(tmp, elemof32(tmp), destination_directory);
            consume_int(MessageBox(NULL, tmp, product_ui_id(), MB_OK | MB_ICONINFORMATION));
            } /*block*/
#endif

            status_consume(file_create_directory(destination_directory));

            tstr_xstrkat(destination_directory, elemof32(destination_directory), FILE_DIR_SEP_TSTR);

            /* Need double CH_NULL termination for SHFileOperation */
            source_filename[tstrlen32(source_filename) + 1] = CH_NULL;
            destination_directory[tstrlen32(destination_directory) + 1] = CH_NULL;

            {
            SHFILEOPSTRUCT shfileop;
            zero_struct(shfileop);
            shfileop.wFunc = FO_COPY;
            shfileop.pFrom = source_filename;
            shfileop.pTo = destination_directory;
            shfileop.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCOPYSECURITYATTRIBS;
            (void) SHFileOperation(&shfileop);
            } /*block*/
        }
    }
}

static void
copy_template_list_to_current_user_Templates(
    _In_z_  PTSTR template_list)
{
    PTSTR this_template = template_list;

    for(;;)
    {
        PTSTR end_tstr;

        /* skip numeric element */
        if(NULL == (end_tstr = tstrchr(this_template, CH_COLON)))
            return; /* malformed list */

        this_template = end_tstr + 1; /* skip delimiter */

        end_tstr = tstrchr(this_template, CH_SEMICOLON);

        if(NULL != end_tstr)
            *end_tstr++ = CH_NULL; /* overwrite delimiter */

        copy_this_template_to_current_user_Templates(this_template);

        if(NULL == end_tstr)
            break;

        this_template = end_tstr;
    }
}

static void
delete_this_file_from_current_user_Templates(
    _In_z_  PCTSTR this_file)
{
    TCHARZ user_directory[BUF_MAX_PATHSTRING];

    if(GetUserTemplatesDirectoryName(user_directory, elemof32(user_directory)))
    {
        TCHARZ user_filename[BUF_MAX_PATHSTRING];

        tstr_xstrkpy(user_filename, elemof32(user_filename), user_directory);
        tstr_xstrkat(user_filename, elemof32(user_filename), FILE_DIR_SEP_TSTR);
        tstr_xstrkat(user_filename, elemof32(user_filename), this_file);

        if(file_is_file(user_filename))
        {
#if 1
            {
            TCHARZ tmp[200 + (BUF_MAX_PATHSTRING * 2)];
            tstr_xstrkpy(tmp, elemof32(tmp), TEXT("Deleting "));
            tstr_xstrkat(tmp, elemof32(tmp), this_file);
            tstr_xstrkat(tmp, elemof32(tmp), TEXT("\nfrom "));
            tstr_xstrkat(tmp, elemof32(tmp), user_directory);
            consume_int(MessageBox(NULL, tmp, product_ui_id(), MB_OK | MB_ICONINFORMATION));
            } /*block*/
#endif

            status_consume(file_remove(user_filename));
        }
    }
}

static void
delete_template_list_from_current_user_Templates(
    _In_z_  PTSTR template_list)
{
    PTSTR this_template = template_list;

    for(;;)
    {
        PTSTR end_tstr;

        /* skip numeric element */
        if(NULL == (end_tstr = tstrchr(this_template, CH_COLON)))
            return; /* malformed list */

        this_template = end_tstr + 1; /* skip delimiter */

        end_tstr = tstrchr(this_template, CH_SEMICOLON);

        if(NULL != end_tstr)
            *end_tstr++ = CH_NULL; /* overwrite delimiter */

        delete_this_file_from_current_user_Templates(this_template);

        if(NULL == end_tstr)
            break;

        this_template = end_tstr;
    }
}

static void
upgrade_Templates(
    _In_z_  PTSTR current_user_template_list,
    _In_z_  PTSTR source_template_list)
{
    delete_template_list_from_current_user_Templates(current_user_template_list);

    copy_template_list_to_current_user_Templates(source_template_list);
}

_Check_return_
static STATUS
ensure_templates_installed(void)
{
    STATUS status = STATUS_OK;
    HKEY hkey;
    DWORD err;

    if(ERROR_SUCCESS != (err = RegOpenKeyExW(HKEY_CURRENT_USER, key_program_wstr, 0, KEY_SET_VALUE, &hkey)))
        return(STATUS_FAIL);

    if(status_ok(status))
    {
        /* Copy standard Template file(s) from application's DefaultUser AppData directory
         * to the current User's Application Data directory.
        */
        TCHARZ source_template_list[BUF_MAX_PATHSTRING];
        TCHARZ current_user_template_list[BUF_MAX_PATHSTRING];

        consume(U32, HklmGetProfileString(TEXT("StandardTemplates"), tstr_empty_string, source_template_list, elemof32(source_template_list)));

        consume(U32, MyGetProfileString(TEXT("InstalledStandardTemplates"), TEXT("0:letter.fwt;0:sheet.fwt"), current_user_template_list, elemof32(current_user_template_list)));

        if(0 != tstrcmp(current_user_template_list, source_template_list))
        {
            int current_user_templates_version = _tstoi(current_user_template_list);

            {
            TCHARZ tmp[BUF_MAX_PATHSTRING];
            tstr_xstrkpy(tmp, elemof32(tmp), (current_user_templates_version == 0) ? TEXT("Installing") : TEXT("Upgrading"));
            tstr_xstrkat(tmp, elemof32(tmp), TEXT(" standard template files in user's Application Data dir"));
            consume_int(MessageBox(NULL, tmp, product_ui_id(), MB_OK | MB_ICONINFORMATION));
            } /*block*/

            /* Remember what will be installed for this user. NB copy_template_list_to_current_user_ThisAppData hacks the list. */
            (void) RegSetValueEx(hkey, TEXT("InstalledStandardTemplates"), 0, REG_SZ, (const BYTE *) source_template_list, (DWORD) ((tstrlen32p1(source_template_list) /*CH_NULL*/)) * sizeof32(TCHAR));

            upgrade_ThisAppData(current_user_template_list, source_template_list, TEXT("Templates"));
        }
    }

    if(status_ok(status))
    {
        /* Copy default Template file(s) from application's Templates directory
         * to the current User's Templates directory.
        */
        TCHARZ source_template_list[BUF_MAX_PATHSTRING];
        TCHARZ current_user_template_list[BUF_MAX_PATHSTRING];

        consume(U32, HklmGetProfileString(TEXT("DefaultTemplate"), tstr_empty_string, source_template_list, elemof32(source_template_list)));

        consume(U32, MyGetProfileString(TEXT("InstalledDefaultTemplate"), TEXT("0:default.fwt"), current_user_template_list, elemof32(current_user_template_list)));

        if(0 != tstrcmp(current_user_template_list, source_template_list))
        {
            int current_user_templates_version = _tstoi(current_user_template_list);

            {
            TCHARZ tmp[BUF_MAX_PATHSTRING];
            tstr_xstrkpy(tmp, elemof32(tmp), (current_user_templates_version == 0) ? TEXT("Installing") : TEXT("Upgrading"));
            tstr_xstrkat(tmp, elemof32(tmp), TEXT(" default template files in user's Templates dir"));
            consume_int(MessageBox(NULL, tmp, product_ui_id(), MB_OK | MB_ICONINFORMATION));
            } /*block*/

            /* Remember what will be installed for this user. NB copy_template_list_to_current_user_template_list hacks the list. */
            (void) RegSetValueEx(hkey, TEXT("InstalledDefaultTemplate"), 0, REG_SZ, (const BYTE *) source_template_list, (DWORD) ((tstrlen32p1(source_template_list) /*CH_NULL*/)) * sizeof32(TCHAR));

            upgrade_Templates(current_user_template_list, source_template_list);
        }
    }

    (void) RegCloseKey(hkey);

    if(status_fail(status))
        reperr_null(status);

    return(status);
}

_Check_return_
static STATUS
host_initialise_user_path_ensure(void)
{
    TCHARZ user_path[BUF_MAX_PATHSTRING];
    STATUS status = STATUS_OK;
    HKEY hkey;
    DWORD err;

    user_path[0] = CH_NULL;

    /* Ensure the program's key is present */
    if(ERROR_SUCCESS != (err = RegOpenKeyExW(HKEY_CURRENT_USER, key_program_wstr, 0, KEY_SET_VALUE, &hkey)))
    {
        DWORD dwDisp;

        if(ERROR_SUCCESS != (err = RegCreateKeyExW(HKEY_CURRENT_USER, key_program_wstr, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &dwDisp)))
        {
            return(STATUS_FAIL);
        }

        (void) RegCloseKey(hkey);

        if(ERROR_SUCCESS != (err = RegOpenKeyExW(HKEY_CURRENT_USER, key_program_wstr, 0, KEY_SET_VALUE, &hkey)))
        {
            return(STATUS_FAIL);
        }
    }

    /* If UserPath registry entry is not set or is empty, ensure that it is created. */
    if( (0 == MyGetProfileString(TEXT("UserPath"), tstr_empty_string, user_path, elemof(user_path))) ||
        (0 == tstrlen32(user_path)) )
    {
        TCHARZ user_directory[BUF_MAX_PATHSTRING];

        if(GetUserAppDataDirectoryName_Fireworkz(user_directory, elemof32(user_directory), TRUE))
        {
            (void) RegSetValueEx(hkey, TEXT("UserPath"), 0, REG_SZ, (const BYTE *) user_directory, (DWORD) ((tstrlen32p1(user_directory) /*CH_NULL*/)) * sizeof32(TCHAR));
        }
    }

    /* Normalise returned user_path *with* trailing FILE_DIR_SEP_CH */
    if( (0 == MyGetProfileString(TEXT("UserPath"), tstr_empty_string, user_path, elemof(user_path))) ||
        (0 == tstrlen32(user_path)) )
    {
        user_path[0] = CH_NULL;
    }
    else
    {
        PTSTR tstr = user_path;

        tstr += tstrlen32(tstr);

        if(tstr[-1] != FILE_DIR_SEP_CH)
        {
            *tstr++ = FILE_DIR_SEP_CH;
            *tstr = CH_NULL;
        }
    }

    status_assert(file_path_set(user_path, FILE_PATH_USER));

    /* Copy some default values over from HKEY_LOCAL_MACHINE
     * to HKEY_CURRENT_USER subkey for ease of modification.
     */
    ensure_key_in_current_user(hkey, TEXT("ReportEnable"));

    (void) RegCloseKey(hkey);

    return(status);
}

/* GetUserDefaultLocaleName only available on Vista and later */

static int
(WINAPI * proc_GetUserDefaultLocaleName) (
    /*_Out_writes_(cchLocaleName)*/ LPWSTR /*lpLocaleName*/,
    /*_In_*/ int /*cchLocaleName*/
) = NULL;

_Check_return_ _Success_(return)
static BOOL
host_get_ui_country_vista(
    _Out_writes_z_(cchReturnBuffer) PTSTR ptzReturnBuffer,
    _InVal_     U32 cchReturnBuffer)
{
    WCHARZ wstr_LocaleName[LOCALE_NAME_MAX_LENGTH];

    if(NULL == proc_GetUserDefaultLocaleName)
    {
        HMODULE hModule = LoadLibrary(TEXT("kernel32.dll"));
        assert(NULL != hModule);
        if(NULL != hModule)
            * (FARPROC *) &proc_GetUserDefaultLocaleName = GetProcAddress(hModule, "GetUserDefaultLocaleName");
    }

    if(NULL == proc_GetUserDefaultLocaleName)
        return(FALSE);

    if(0 == (* proc_GetUserDefaultLocaleName) (wstr_LocaleName, elemof32(wstr_LocaleName)))
        return(FALSE);

    if(0 == wstrncmp(wstr_LocaleName, L"de", 2))
    {
        if( (CH_NULL == wstr_LocaleName[2]) || ('-' == wstr_LocaleName[2]) )
        {
            tstr_xstrkpy(ptzReturnBuffer, cchReturnBuffer, TEXT("Germany"));
            return(TRUE);
        }
    }

    if(0 == wcsncmp(wstr_LocaleName, L"fr", 2))
    {
        if( (CH_NULL == wstr_LocaleName[2]) || ('-' == wstr_LocaleName[2]) )
        {
            tstr_xstrkpy(ptzReturnBuffer, cchReturnBuffer, TEXT("France"));
            return(TRUE);
        }
    }

    return(FALSE);
}

/* XP-compatible fallback */

_Check_return_ _Success_(return)
static BOOL
host_get_ui_country_xp(
    _Out_writes_z_(cchReturnBuffer) PTSTR ptzReturnBuffer,
    _InVal_     U32 cchReturnBuffer)
{
    const LANGID langid = GetUserDefaultUILanguage();
    const WORD primary_langid = PRIMARYLANGID(langid);

    if(LANG_GERMAN == primary_langid)
    {
        tstr_xstrkpy(ptzReturnBuffer, cchReturnBuffer, TEXT("Germany"));
        return(TRUE);
    }

    if(LANG_FRENCH == primary_langid)
    {
        tstr_xstrkpy(ptzReturnBuffer, cchReturnBuffer, TEXT("France"));
        return(TRUE);
    }

    return(FALSE);
}

static void
host_get_ui_country(
    _Out_writes_z_(cchReturnBuffer) PTSTR ptzReturnBuffer,
    _InVal_     U32 cchReturnBuffer,
    _In_z_      PCTSTR ptzDefault)
{
    /* this takes priority if set */
    consume(U32, MyGetProfileString(TEXT("Country"), tstr_empty_string, ptzReturnBuffer, cchReturnBuffer));

    if(CH_NULL != ptzReturnBuffer[0])
        return;

    if(host_get_ui_country_vista(ptzReturnBuffer, cchReturnBuffer))
        return;

    if(host_get_ui_country_xp(ptzReturnBuffer, cchReturnBuffer))
        return;

    tstr_xstrkpy(ptzReturnBuffer, cchReturnBuffer, ptzDefault);
}

static void
host_initialise_admin_path(void)
{
    TCHARZ admin_path[BUF_MAX_PATHSTRING];

    if(0 != MyGetProfileString(TEXT("AdminPath"), tstr_empty_string, admin_path, elemof32(admin_path)))
    {
        PTSTR tstr = admin_path;

        tstr += tstrlen32(tstr);

        if((tstr != admin_path) && (tstr[-1] != FILE_DIR_SEP_CH))
        {   /* append to non-empty path if needed */
            *tstr++ = FILE_DIR_SEP_CH;
            *tstr = CH_NULL;
        }
    }
    else
        admin_path[0] = CH_NULL;

    if(CH_NULL != admin_path[0])
    {
        trace_2(TRACE_OUT | TRACE_ANY, TEXT("file_path_set(%d): %s"), FILE_PATH_ADMIN, report_tstr(admin_path));
        status_assert(file_path_set(admin_path, FILE_PATH_ADMIN));
    }
}

static void
host_initialise_resources_path(
    _In_z_      PCTSTR module_path)
{
    TCHARZ path_buffer[BUF_MAX_PATHSTRING * 4];

    if(0 != MyGetProfileString(TEXT("ResourcesPath"), tstr_empty_string, path_buffer, elemof32(path_buffer)))
    {
        PTSTR tstr = path_buffer;

        tstr += tstrlen32(tstr);

        if( (tstr != path_buffer) && (tstr[-1] != FILE_DIR_SEP_CH) )
        {   /* append to non-empty path if needed */
            *tstr++ = FILE_DIR_SEP_CH;
            *tstr = CH_NULL;
        }
    }
    else
        path_buffer[0] = CH_NULL;

    if(CH_NULL == path_buffer[0])
    {
        /* resources path is specified country first, then default country, then neutral */
        TCHARZ resources_path_common[BUF_MAX_PATHSTRING];
        const PCTSTR default_country = TEXT("UK");
        TCHARZ country[BUF_MAX_PATHSTRING];

        tstr_xstrkpy(resources_path_common, elemof32(resources_path_common), module_path);
        tstr_xstrkat(resources_path_common, elemof32(resources_path_common), TEXT("Resources") FILE_DIR_SEP_TSTR);

        host_get_ui_country(country, elemof32(country), default_country);

        {
        TCHARZ resources_path_country[BUF_MAX_PATHSTRING];
        tstr_xstrkpy(resources_path_country, elemof32(resources_path_country), resources_path_common);
        tstr_xstrkat(resources_path_country, elemof32(resources_path_country), country);
        tstr_xstrkat(resources_path_country, elemof32(resources_path_country), FILE_DIR_SEP_TSTR);

        tstr_xstrkpy(path_buffer, elemof32(path_buffer), resources_path_country);
        tstr_xstrkat(path_buffer, elemof32(path_buffer), FILE_PATH_SEP_TSTR);
        } /*block*/

        /* ensure that default country is used as fallback for language-dependent resources iff different */
        if(0 != _tcscmp(country, default_country))
        {
            TCHARZ resources_path_default_country[BUF_MAX_PATHSTRING];
            tstr_xstrkpy(resources_path_default_country, elemof32(resources_path_default_country), resources_path_common);
            tstr_xstrkat(resources_path_default_country, elemof32(resources_path_default_country), default_country);
            tstr_xstrkat(resources_path_default_country, elemof32(resources_path_default_country), FILE_DIR_SEP_TSTR);

            tstr_xstrkat(path_buffer, elemof32(path_buffer), resources_path_default_country);
            tstr_xstrkat(path_buffer, elemof32(path_buffer), FILE_PATH_SEP_TSTR);
        }

        {
        TCHARZ resources_path_neutral[BUF_MAX_PATHSTRING];
        tstr_xstrkpy(resources_path_neutral, elemof32(resources_path_neutral), resources_path_common);
        tstr_xstrkat(resources_path_neutral, elemof32(resources_path_neutral), TEXT("Neutral") FILE_DIR_SEP_TSTR);

        tstr_xstrkat(path_buffer, elemof32(path_buffer), resources_path_neutral);
        } /*block*/
    }

    status_assert(file_path_set(path_buffer, FILE_PATH_RESOURCES));
}

static void
host_initialise_templates_path(void)
{
    TCHARZ full_path_buffer[BUF_MAX_PATHSTRING * 4];
    TCHARZ path_buffer[BUF_MAX_PATHSTRING];
    U32 full_path_buffer_len;

    full_path_buffer[0] = CH_NULL;

    /* First add All Users Application Data / <App> / Templates directory */
    /* We don't install anything here, but consider it anyway as an Admin may have added data */
    if(GetCommonAppDataDirectoryName(path_buffer, elemof32(path_buffer)))
    {
        tstr_xstrkat(path_buffer, elemof32(path_buffer),
                        FILE_DIR_SEP_TSTR TEXT("Colton Software")
                        FILE_DIR_SEP_TSTR TEXT("Fireworkz")
                        FILE_DIR_SEP_TSTR TEMPLATES_SUBDIR);

        tstr_xstrkat(full_path_buffer, elemof32(path_buffer), path_buffer);
        tstr_xstrkat(full_path_buffer, elemof32(path_buffer), FILE_PATH_SEP_TSTR);
    }

    /* Then add All Users Templates directory */
    /* Ditto regarding install */
    if(GetCommonTemplatesDirectoryName(path_buffer, elemof32(path_buffer)))
    {
        tstr_xstrkat(full_path_buffer, elemof32(path_buffer), path_buffer);
        tstr_xstrkat(full_path_buffer, elemof32(path_buffer), FILE_PATH_SEP_TSTR);
    }

    /* Then add User's Templates directory */
    /* We do install a single Fireworkz template here to allow New in an Explorer menu, but User may have added more */
    if(GetUserTemplatesDirectoryName(path_buffer, elemof32(path_buffer)))
    {
        tstr_xstrkat(full_path_buffer, elemof32(path_buffer), path_buffer);
        tstr_xstrkat(full_path_buffer, elemof32(path_buffer), FILE_PATH_SEP_TSTR);
    }

    full_path_buffer_len = tstrlen32(full_path_buffer);

    if( (0 != full_path_buffer_len) && (FILE_PATH_SEP_CH == full_path_buffer[full_path_buffer_len-1]) )
        full_path_buffer[full_path_buffer_len-1] = CH_NULL;

    status_assert(file_path_set(full_path_buffer, FILE_PATH_TEMPLATES));
}

extern void
host_initialise_file_paths(void)
{
    TCHARZ module_path[BUF_MAX_PATHSTRING];

    status_assert(host_initialise_user_path_ensure());

    host_initialise_admin_path();

    if(0 != MyGetProfileString(TEXT("Directory"), tstr_empty_string, module_path, elemof32(module_path)))
    {
        PTSTR tstr = module_path;

        tstr += tstrlen32(tstr);

        if( (tstr != module_path) && (tstr[-1] != FILE_DIR_SEP_CH) )
        {   /* append to non-empty path if needed */
            *tstr++ = FILE_DIR_SEP_CH;
            *tstr = CH_NULL;
        }
    }
    else
    {
        TCHARZ module_file_name[BUF_MAX_PATHSTRING];
        GetModuleFileName(GetInstanceHandle(), module_file_name, elemof32(module_file_name));
        file_dirname(module_path, module_file_name); /* has trailing DIR_SEP */
    }

    host_initialise_resources_path(module_path);

    host_initialise_templates_path();

    file_build_paths();
}

/*ncr*/
static BOOL
decode_command_line_options(
    _In_opt_z_  PCTSTR command_line,
    _InVal_     int pass)
{
    /* try trivial parsing of command line */
    for(;;)
    {
        PCTSTR p_next;

        if(NULL == command_line)
            break;

        StrSkipSpaces(command_line);

        if(CH_NULL == *command_line)
            break;

        p_next = tstrchr(command_line, CH_SPACE);

        if((CH_HYPHEN_MINUS == *command_line) || (CH_FORWARDS_SLASH == *command_line))
        {
            /* keywords */
            switch(command_line[1])
            {
            case 'n':
                if(pass == 1)
                    /* started up for DDE */
                    g_started_for_dde = TRUE;
                break;

            default:
                break;
            }
        }
        else
        {
            PCTSTR filename = command_line;
            TCHARZ buffer[BUF_MAX_PATHSTRING];
            U32 len;

            if(*filename == CH_QUOTATION_MARK)
            {
                filename++;
                p_next = tstrchr(filename, CH_QUOTATION_MARK);
                if(NULL != p_next)
                {
                    len = PtrDiffElemU32(p_next, filename);
                    memcpy32(buffer, filename, len);
                    buffer[len] = CH_NULL;
                    filename = buffer;
                    p_next++; /* skip quote */
                }
            }
            else if(NULL != p_next)
            {
                len = PtrDiffElemU32(p_next, filename);
                memcpy32(buffer, filename, len);
                buffer[len] = CH_NULL;
                filename = buffer;
            }

            if(pass == 2)
                status_break(load_file_for_windows_startup_rl(filename));
        }

        command_line = p_next;
    }

    return(TRUE);
}

#include <setjmp.h>

static BOOL event_loop_jmp_set = FALSE;
static jmp_buf event_loop_jmp_buf;

extern void
host_longjmp_to_event_loop(int val)
{
    if(event_loop_jmp_set)
        longjmp(event_loop_jmp_buf, val);
}

static void
host_report_and_trace_enable(void)
{
    { /* allow application to run without any report info */
    BOOL enable = FALSE;
    TCHARZ env_value[BUF_MAX_PATHSTRING];
    if(0 != MyGetProfileString(TEXT("ReportEnable"), tstr_empty_string, env_value, elemof32(env_value)))
        enable = (0 != _tstoi(env_value)); /*atoi*/
    report_enable(enable);
    } /* block */

#if TRACE_ALLOWED
    { /* Allow a version built with TRACE_ALLOWED to run on an end-user's system without outputting any trace info */
    BOOL enable = FALSE;
    TCHARZ env_value[BUF_MAX_PATHSTRING];
    if(0 != MyGetProfileString(TEXT("TraceEnable"), tstr_empty_string, env_value, elemof32(env_value)))
        enable = (0 != _tstoi(env_value)); /*atoi*/
    if(enable)
        trace_on();
    else
        trace_disable();
    } /* block */
#endif /*TRACE_ALLOWED*/
}

_Check_return_
static BOOL
main_init(
    _In_z_      PTSTR ptzCmdLine_in)
{
    /* determine which platform we are running on */
    /*host_os_version_determine();*/ /* very early indeed */
    report_timing_enable(TRUE);

    (void) HeapSetInformation(GetProcessHeap(), HeapEnableTerminationOnCorruption, NULL, 0);

#if CHECKING && defined(_DEBUG) && 0
    int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);// Get current flag
    tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_DELAY_FREE_MEM_DF; // Turn on checking bits
    _CrtSetDbgFlag(tmpFlag); // Set flag to the new value
#endif

    host_report_and_trace_enable();

    /* set locale for isalpha etc. (ctype.h functions) */
    trace_1(TRACE_OUT | TRACE_ANY, TEXT("main: initial CTYPE locale is %hs"), setlocale(LC_CTYPE, NULL));
    trace_1(TRACE_OUT | TRACE_ANY, TEXT("main: initial TIME  locale is %hs"), setlocale(LC_TIME,  NULL));
    consume_ptr(setlocale(LC_CTYPE, "C")); /* standard ASCII NB NOT "ISO8859-1" (ISO Latin-1) */
    consume_ptr(setlocale(LC_TIME,  "C")); /* standard ASCII NB NOT "ISO8859-1" (ISO Latin-1) */

    _tzset();

    if(status_fail(startup_t5_application_1()))
        return(FALSE);

    status_consume(aligator_init());

    /* parse initial set of options */
    (void) decode_command_line_options(ptzCmdLine_in, 1);

    file_startup();

    /* first off, allow punter to place resources elsewhere. Installation will have inserted our path */
    host_initialise_file_paths();

    /* Make error messages available for startup */
    resource_startup();

    status_assert(resource_init(OBJECT_ID_SKEL, NULL, LOAD_RESOURCES));

    gdiplus_startup(); /* New Dial Solution Draw rendering DLLs require GDI+, as do we too for PNG etc. handling */

    host_create_default_palette();

    get_user_info();

    host_dde_startup();

    if(!g_started_for_dde)
        splash_window_create(HWND_DESKTOP, 0);

    /* Startup the application. Any errors will have been reported */
    if(status_fail(startup_t5_application_2()))
        return(FALSE);

    if(!decode_command_line_options(ptzCmdLine_in, 2))
        return(FALSE);

    /* If nothing has been loaded (except when started as DDE server) then open a template - or the template selector */
    if(some_document_windows() || g_started_for_dde)
        return(TRUE);

    if(status_fail(ensure_templates_installed()))
        return(FALSE);

#if CHECKING && 1
    /* try open existing first, if cancelled, try create from template - leave this way for SKS testing */
    if(status_fail(object_call_id(OBJECT_ID_FILE, P_DOCU_NONE, T5_CMD_OPEN_DOCUMENT, P_DATA_NONE)))
        status_assert(object_call_id(OBJECT_ID_SKEL, P_DOCU_NONE, T5_CMD_NEW_DOCUMENT_INTRO, P_DATA_NONE));
#else
    /* try create from template first, if cancelled, try open existing */
    if(status_fail(object_call_id(OBJECT_ID_SKEL, P_DOCU_NONE, T5_CMD_NEW_DOCUMENT_INTRO, P_DATA_NONE)))
        status_assert(object_call_id(OBJECT_ID_FILE, P_DOCU_NONE, T5_CMD_OPEN_DOCUMENT, P_DATA_NONE));
#endif

    return(TRUE);
}

extern int
WINAPI
_tWinMain(
#if !defined(WINVER_MAXVER) || (WINVER_MAXVER < 0x0603)
    /* old SAL */
    __in        HINSTANCE hInstance_in,
    __in_opt    HINSTANCE hInstance_previous_in,
    __in_opt    PTSTR ptzCmdLine_in,
    __in        int nCmdShow_in
#else
    /* new SAL */
    _In_        HINSTANCE hInstance_in,
    _In_opt_    HINSTANCE hInstance_previous_in,
    _In_        PTSTR ptzCmdLine_in,
    _In_        int nCmdShow_in
#endif
    )
{
    BOOL looping = TRUE;

    _g_hInstance = hInstance_in;
    g_hInstancePrev = hInstance_previous_in;
    g_nCmdShow = nCmdShow_in;

    if(!main_init(ptzCmdLine_in))
        goto fallout;

#if defined(__cplusplus)
    while_constant(1)
    {
    /* don't do this kind of shit!!! */
#else
    /* set up a point we can longjmp back to on serious error */
    switch(setjmp(event_loop_jmp_buf))
    {
    default:
        /* returned here from exception - tidy up as necessary */
        status_assert(maeve_service_event(P_DOCU_NONE, T5_MSG_HAD_SERIOUS_ERROR, P_DATA_NONE));

        trace_0(TRACE_APP_SKEL, TEXT("winmain: Starting to poll for messages again after serious error"));
        break;

    case 0:
        event_loop_jmp_set = TRUE;
#endif /* __cplusplus */

        trace_0(TRACE_APP_SKEL, TEXT("winmain: Starting to poll for messages"));
        break;
    }

    if(!some_document_windows() && !g_started_for_dde)
        goto fallout;

    while(looping)
    {
        WM_EVENT res = wm_event_get(FALSE /*fgNullEventsWanted*/);

        if(WM_EVENT_PROCESSED != res)
            break;
    }

fallout:
    looping = FALSE; /* don't run the event loop if we get a longjmp() back above during closedown */

    docno_close_all();

    t5_do_exit();

    return(EXIT_SUCCESS);
}

extern void
host_shutdown(void)
{
    splash_window_remove();

    host_destroy_default_palette();

    gdiplus_shutdown();
}

#endif /* WINDOWS */

/* end of windows/winmain.c */
