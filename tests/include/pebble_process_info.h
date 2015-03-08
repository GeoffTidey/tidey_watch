#pragma once

#include <stdint.h>

//! @addtogroup Foundation
//! @{
//!   @addtogroup App
//!   @{

//! Application metadata flags.
//! Flags can be combined using the `|` operator.
//! @see PBL_PROCESS_INFO
typedef enum {
  //! Use to indicate the process is a "standard" app.
  //! The system will show the app in the main menu.
  PROCESS_INFO_STANDARD_APP = 0,
  //! Use to indicate the process is a watchface.
  //! The system will show the process in the watchfaces menu.
  PROCESS_INFO_WATCH_FACE = 1 << 0,
  //! Use to hide the process.
  PROCESS_INFO_VISIBILITY_HIDDEN = 1 << 1,
  //! Use to hide the process, unless there is ongoing communication with
  //! the companion smartphone application.
  PROCESS_INFO_VISIBILITY_SHOWN_ON_COMMUNICATION = 1 << 2,
  //! Use to indicate the process allows Javascript API access
  PROCESS_INFO_ALLOW_JS = 1 << 3,
  //! Use to indicate the process should have a worker.bin installed as well.
  PROCESS_INFO_HAS_WORKER = 1 << 4,
} PebbleProcessInfoFlags;

//!   @} // group App
//! @} // group Foundation

// struct PebbleProcessInfo change log
// ================================
// struct_version (little endian):
// 0x0800 -- sdk_version and process_version uint16_t fields added (Grand Slam / 1.7)
// .major:0x08 .minor:0x01 -- all version fields split up into minor/major; uuid field appended (Junior Whopper / 2.0?)
// .major:0x08 .minor:0x02 -- 2.0, added resource crc and resource timestamp
// .major:0x09 .minor:0x00 -- 2.0, no more reloc_list_start
// .major:0x10 .minor:0x00 -- 2.0, added virtual_size
#define PROCESS_INFO_CURRENT_STRUCT_VERSION_MAJOR 0x10
#define PROCESS_INFO_CURRENT_STRUCT_VERSION_MINOR 0x0

// process info version for last know 1.x
// let this be a warning to engineers everywhere
// who want to design a system with fancy versioning and
// support
#define PROCESS_INFO_LEGACY_STRUCT_VERSION_MAJOR 0x08

// SDK change log
// ================================
// sdk.major:0x4 .minor:0x0  -- Bump the SDK version to make 1.x and 2.x apps distinguishable
// sdk.major:0x5 .minor:0x0  -- Bump the SDK version for breaking AppMessage changes b/t 2.x alpha and beta releases
// sdk.major:0x5 .minor:0x1  -- Added additional API functions (MenuLayer callbacks)
// sdk.major:0x5 .minor:0x2  -- Changed app heap double free behaviour.
// sdk.major:0x5 .minor:0x3  -- Added number_window_get_window (API v2.0 revision 12)
// sdk.major:0x5 .minor:0x4  -- Added gbitmap_create_blank (API v2.0 revision 13) and click_recognizer_is_repeating (rev 14)
// sdk.major:0x5 .minor:0x5  -- Added accel_raw_data_service_subscribe and related types (rev 15)
// sdk.major:0x5 .minor:0x6  -- Added background worker APIs (rev 16)
// sdk.major:0x5 .minor:0x7  -- Added heap_bytes_free / heap_bytes_used (rev 17)
// sdk.major:0x5 .minor:0x8  -- Added compass APIs (rev 18)
// sdk.major:0x5 .minor:0x9  -- Added Uuid utility APIs (rev 19)
// sdk.major:0x5 .minor:0xa  -- Fixed gpath behaviour, added gpath_draw_filled_legacy. (rev 20)
// sdk.major:0x5 .minor:0xb  -- Added custom animation curves (rev 21)
// sdk.major:0x5 .minor:0xc  -- Added API for model, color and firmware version of watch (rev 22)
// sdk.major:0x5 .minor:0xd  -- Added direct access to frame buffer (rev 23)
// sdk.major:0x5 .minor:0xe  -- Added wakeup API, app_launch_reason (rev 24)
// sdk.major:0x5 .minor:0xf  -- Added clock_is_timezone_set in preparation for timezone support (rev 25)
// sdk.major:0x5 .minor:0x10 -- Added the first i18n API: get_locale (rev 26)
// sdk.major:0x5 .minor:0x11 -- Added second i18n API: setlocale (rev 27)
// sdk.major:0x5 .minor:0x13 -- Export mktime (rev 29)
#define PROCESS_INFO_CURRENT_SDK_VERSION_MAJOR 0x5
#define PROCESS_INFO_CURRENT_SDK_VERSION_MINOR 0x13

#define PROCESS_NAME_BYTES 32
#define COMPANY_NAME_BYTES 32

//! @internal
//! Version data structure with minor & major versions: When making non-backwards-compatible changes,
//! the major version should get bumped. When making a change (e.g. to the PebbleProcessInfo struct) that is backwards
//! compatible (e.g. adding a field at the end), you should only bump the minor version.
typedef struct __attribute__((__packed__)) {
  uint8_t major; //!< "compatibility" version number
  uint8_t minor;
} Version;

//! @return 0 if a and b are equal, >0 if a > b, <0 if b > a
int version_compare(Version a, Version b);

//! @internal
// WARNING: changes in this struct must be reflected in:
// - tintin/waftools/inject_metadata.py
// - iOS/PebblePrivateKit/PebblePrivateKit/PBBundle.m
typedef struct __attribute__((__packed__)) {
  char header[8];                   //!< Sentinal value, should always be 'PBLAPP'
  Version struct_version;           //!< version of this structure's format
  Version sdk_version;              //!< version of the SDK used to build this process
  Version process_version;          //!< version of the process
  uint16_t load_size;               //!< size of the binary in flash, including this metadata but not the reloc table
  uint32_t offset;                  //!< The entry point of this executable
  uint32_t crc;                     //!< CRC of the data only, ie, not including this struct or the reloc table at the end
  char name[PROCESS_NAME_BYTES];    //!< Name to display on the menu
  char company[COMPANY_NAME_BYTES]; //!< Name of the maker of this process
  uint32_t icon_resource_id;        //!< Resource ID within this bank to use as a 32x32 icon
  uint32_t sym_table_addr;          //!< The system will poke the sdk's symbol table address into this field on load
  uint32_t flags;                   //!< Bitwise OR of PebbleProcessInfoFlags
  uint32_t num_reloc_entries;       //!< The number of entries in the address relocation list
  struct __attribute__((__packed__)) {
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
    uint8_t byte4;
    uint8_t byte5;
    uint8_t byte6;
    uint8_t byte7;
    uint8_t byte8;
    uint8_t byte9;
    uint8_t byte10;
    uint8_t byte11;
    uint8_t byte12;
    uint8_t byte13;
    uint8_t byte14;
    uint8_t byte15;
  } uuid;                           //!< The process's UUID
  uint32_t resource_crc;            //!< CRC of the resource data only
  uint32_t resource_timestamp;      //!< timestamp of the resource data
  uint16_t virtual_size;            //!< The total amount of memory used by the process (.text + .data + .bss)
} PebbleProcessInfo;

//! @internal
typedef struct __attribute__((__packed__)) {
  char header[8];                   //!< Sentinal value, should always be 'PBLAPP'
  Version struct_version;           //!< version of this structure's format
  Version sdk_version;              //!< version of the SDK used to build this process
  Version process_version;          //!< version of the process
  uint16_t load_size;               //!< size of the binary in flash, including this metadata but not the reloc table
  uint32_t offset;                  //!< The entry point of this executable
  uint32_t crc;                     //!< CRC of the data only, ie, not including this struct or the reloc table at the end
  char name[PROCESS_NAME_BYTES];    //!< Name to display on the menu
  char company[COMPANY_NAME_BYTES]; //!< Name of the maker of this process
  uint32_t icon_resource_id;        //!< Resource ID within this process's bank to use as a 32x32 icon
  uint32_t sym_table_addr;          //!< The system will poke the sdk's symbol table address into this field on load
  uint32_t flags;                   //!< Bitwise OR of PebbleProcessInfoFlags
  uint32_t reloc_list_start;        //!< The offset of the address relocation list
  uint32_t num_reloc_entries;       //!< The number of entries in the address relocation list
  struct __attribute__((__packed__)) {
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
    uint8_t byte4;
    uint8_t byte5;
    uint8_t byte6;
    uint8_t byte7;
    uint8_t byte8;
    uint8_t byte9;
    uint8_t byte10;
    uint8_t byte11;
    uint8_t byte12;
    uint8_t byte13;
    uint8_t byte14;
    uint8_t byte15;
  } uuid;                           //!< The process's UUID
} LegacyPebbleAppInfo;


