#pragma once

#include <locale.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "pebble_warn_unsupported_functions.h"

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))

typedef struct ListNode {
  struct ListNode* next;
  struct ListNode* prev;
} ListNode;

#define IS_SIGNED(var) (__builtin_choose_expr( \
  __builtin_types_compatible_p(__typeof__(var), unsigned char), false, \
  __builtin_choose_expr( \
  __builtin_types_compatible_p(__typeof__(var), unsigned short), false, \
  __builtin_choose_expr( \
  __builtin_types_compatible_p(__typeof__(var), unsigned int), false, \
  __builtin_choose_expr( \
  __builtin_types_compatible_p(__typeof__(var), unsigned long), false, \
  __builtin_choose_expr( \
  __builtin_types_compatible_p(__typeof__(var), unsigned long long), false, true))))) \
)

//! @addtogroup Foundation
//! @{

//! @addtogroup WatchInfo
//! \brief Provides information about the watch itself.
//!
//! This API provides access to information such as the watch model, watch color
//! and watch firmware version.
//! @{

//! The different watch models.
typedef enum {
  WATCH_INFO_MODEL_UNKNOWN, //!< Unknown model
  WATCH_INFO_MODEL_PEBBLE_ORIGINAL, //!< Original Pebble
  WATCH_INFO_MODEL_PEBBLE_STEEL //!< Pebble Steel
} WatchInfoModel;

//! The different watch colors.
typedef enum {
  WATCH_INFO_COLOR_UNKNOWN = 0, //!< Unknown color
  WATCH_INFO_COLOR_BLACK = 1, //!< Black
  WATCH_INFO_COLOR_WHITE = 2, //!< White
  WATCH_INFO_COLOR_RED = 3, //!< Red
  WATCH_INFO_COLOR_ORANGE = 4, //!< Orange
  WATCH_INFO_COLOR_GREY = 5, //!< Grey
  WATCH_INFO_COLOR_STAINLESS_STEEL = 6, //!< Stainless Steel
  WATCH_INFO_COLOR_MATTE_BLACK = 7, //!< Matte Black
  WATCH_INFO_COLOR_BLUE = 8, //!< Blue
  WATCH_INFO_COLOR_GREEN = 9, //!< Green
  WATCH_INFO_COLOR_PINK = 10 //!< Pink
} WatchInfoColor;

//! Data structure containing the version of the firmware running on the watch.
//! The version of the firmware has the form X.[X.[X]]. If a version number is not present it will be 0.
//! For example: the version numbers of 2.4.1 are 2, 4, and 1. The version numbers of 2.4 are 2, 4, and 0.
typedef struct {
  uint8_t major; //!< Major version number
  uint8_t minor; //!< Minor version number
  uint8_t patch; //!< Patch version number
} WatchInfoVersion;

//! Provides the model of the watch.
//! @return {@link WatchInfoModel} representing the model of the watch.
WatchInfoModel watch_info_get_model(void);

//! Provides the version of the firmware running on the watch.
//! @return {@link WatchInfoVersion} representing the version of the firmware running on the watch.
WatchInfoVersion watch_info_get_firmware_version(void);

//! Provides the color of the watch.
//! @return {@link WatchInfoColor} representing the color of the watch.
WatchInfoColor watch_info_get_color(void);

//! @} // group WatchInfo

//! @addtogroup Math
//! @{

//! The largest value that can result from a call to \ref sin_lookup or \ref cos_lookup.
//! For a code example, see the detailed description at the top of this chapter: \ref Math
#define TRIG_MAX_RATIO 0xffff

//! Angle value that corresponds to 360 degrees or 2 PI radians
//! @see \ref sin_lookup
//! @see \ref cos_lookup
#define TRIG_MAX_ANGLE 0x10000

//! Look-up the sine of the given angle from a pre-computed table.
//! @param angle The angle for which to compute the cosine.
//! The angle value is scaled linearly, such that a value of 0x10000 corresponds to 360 degrees or 2 PI radians.
int32_t sin_lookup(int32_t angle);

//! Look-up the cosine of the given angle from a pre-computed table.
//! This is equivalent to calling `sin_lookup(angle + MAX_ANGLE / 4)`.
//! @param angle The angle for which to compute the cosine.
//! The angle value is scaled linearly, such that a value of 0x10000 corresponds to 360 degrees or 2 PI radians.
int32_t cos_lookup(int32_t angle);

//! Look-up the arctangent of a given x, y pair
//! The angle value is scaled linearly, such that a value of 0x10000 corresponds to 360 degrees or 2 PI radians.
int32_t atan2_lookup(int16_t y, int16_t x);

//! @} // group Math

//! @addtogroup WallTime Wall Time
//!   \brief Functions, data structures and other things related to wall clock time.
//!
//! This module contains utilities to get the current time and create strings with formatted
//! dates and times.
//! @{

//! Weekday values
typedef enum {
  TODAY = 0,  //!< Today
  SUNDAY,     //!< Sunday
  MONDAY,     //!< Monday
  TUESDAY,    //!< Tuesday
  WEDNESDAY,  //!< Wednesday
  THURSDAY,   //!< Thursday
  FRIDAY,     //!< Friday
  SATURDAY,   //!< Saturday
} WeekDay;

//! Copies a time string into the buffer, formatted according to the user's time display preferences (such as 12h/24h
//! time).
//! Example results: "7:30" or "15:00". 
//! @note AM/PM are also outputted with the time if the user's preference is 12h time.
//! @param[out] buffer A pointer to the buffer to copy the time string into
//! @param size The maximum size of buffer
void clock_copy_time_string(char *buffer, uint8_t size);

//! Gets the user's 12/24h clock style preference.
//! @return `true` if the user prefers 24h-style time display or `false` if the
//! user prefers 12h-style time display.
bool clock_is_24h_style(void);

//! Converts a (day, hour, minute) specification to a UTC timestamp occurring in the future
//! Always returns a timestamp for the next occurring instance,
//! example: specifying TODAY@14:30 when it is 14:40 will return a timestamp for 7 days from
//! now at 14:30
//! @note This function does not support Daylight Saving Time (DST) changes, events scheduled
//! during a DST change will be off by an hour.
//! @param day WeekDay day of week including support for specifying TODAY
//! @param hour hour specified in 24-hour format [0-23]
//! @param minute minute [0-59]
time_t clock_to_timestamp(WeekDay day, int hour, int minute);

//! Checks if timezone is currently set, otherwise gmtime == localtime.
//! @note This function was added in preparation of timezone support, 
//! currently always returns false
//! @return `true` if timezone has been set, false otherwise
bool clock_is_timezone_set(void);

//! @} // group WallTime

//! @addtogroup EventService
//! @{

//! @addtogroup BluetoothConnectionService
//! \brief Determine when Pebble is connected to the phone
//!
//! The BluetoothConnectionService allows your app to know whether Pebble is connected to the phone.
//! You can ask the system for this information at a given time or you can register
//! to receive events every time Pebble connects or disconnects to the phone.
//! @{

//! Callback type for bluetooth connection events
//! @param connected true on bluetooth connection, false on disconnection
typedef void (*BluetoothConnectionHandler)(bool connected);

//! Query the bluetooth connection service for the current connection status
//! @return true if connected, false otherwise
bool bluetooth_connection_service_peek(void);

//! Subscribe to the bluetooth event service. Once subscribed, the handler gets called
//! on every bluetooth connection event.
//! @param handler A callback to be executed on connection event
void bluetooth_connection_service_subscribe(BluetoothConnectionHandler handler);

//! Unsubscribe from the bluetooth event service. Once unsubscribed, the previously registered
//! handler will no longer be called.
void bluetooth_connection_service_unsubscribe(void);

//! @} // group BluetoothConnectionService

//! @addtogroup BatteryStateService
//!
//! \brief Determines when the battery state changes
//!
//! The BatteryStateService API lets you know when the battery state changes, that is,
//! its current charge level, whether it is plugged and charging. It uses the
//! BatteryChargeState structure to describe the current power state of Pebble.
//!
//! Refer to /Examples/watchfaces/classio-battery-connection,
//! which demonstrates using the battery state service in a watchface.
//! @{

//! Structure for retrieval of the battery charge state
typedef struct {
  //! A percentage (0-100) of how full the battery is
  uint8_t charge_percent;
  //! True if the battery is currently being charged. False if not.
  bool is_charging;
  //! True if the charger cable is connected. False if not.
  bool is_plugged;
} BatteryChargeState;

//! Callback type for battery state change events
//! @param charge the state of the battery \ref BatteryChargeState
typedef void (*BatteryStateHandler)(BatteryChargeState charge);

//! Subscribe to the battery state event service. Once subscribed, the handler gets called
//! on every battery state change
//! @param handler A callback to be executed on battery state change event
void battery_state_service_subscribe(BatteryStateHandler handler);

//! Unsubscribe from the battery state event service. Once unsubscribed, the previously registered
//! handler will no longer be called.
void battery_state_service_unsubscribe(void);

//! Peek at the last known battery state.
//! @return a \ref BatteryChargeState containing the last known data
BatteryChargeState battery_state_service_peek(void);

//! @} // group BatteryStateService

//! @addtogroup AccelerometerService
//!
//! \brief Using the Pebble accelerometer
//!
//! The AccelerometerService enables the Pebble accelerometer to detect taps,
//! perform measures at a given frequency, and transmit samples in batches to save CPU time
//! and processing.
//!
//! For available code samples, see
//! Examples/watchapps/feature_accel_discs
//! @{

//! A single accelerometer sample for all three axes including timestamp and
//! vibration rumble status.
typedef struct __attribute__((__packed__)) AccelData {
  //! acceleration along the x axis
  int16_t x;
  //! acceleration along the y axis
  int16_t y;
  //! acceleration along the z axis
  int16_t z;

  //! true if the watch vibrated when this sample was collected
  bool did_vibrate;

  //! timestamp, in milliseconds
  uint64_t timestamp;
} AccelData;

//! A single accelerometer sample for all three axes
typedef struct __attribute__((__packed__)) {
  //! acceleration along the x axis
  int16_t x;
  //! acceleration along the y axis
  int16_t y;
  //! acceleration along the z axis
  int16_t z;
} AccelRawData;

typedef enum {
  //! Accelerometer's X axis. The positive direction along the X axis goes
  //! toward the right of the watch.
  ACCEL_AXIS_X = 0,
  //! Accelerometer's Y axis. The positive direction along the Y axis goes
  //! toward the top of the watch.
  ACCEL_AXIS_Y = 1,
  //! Accelerometer's Z axis. The positive direction along the Z axis goes
  //! vertically out of the watchface.
  ACCEL_AXIS_Z = 2,
} AccelAxisType;

//! Callback type for accelerometer data events
//! @param data Pointer to the collected accelerometer samples.
//! @param num_samples the number of samples stored in data.
typedef void (*AccelDataHandler)(AccelData *data, uint32_t num_samples);

//! Callback type for accelerometer raw data events
//! @param data Pointer to the collected accelerometer samples.
//! @param num_samples the number of samples stored in data.
//! @param timestamp the timestamp, in ms, of the first sample.
typedef void (*AccelRawDataHandler)(AccelRawData *data, uint32_t num_samples, uint64_t timestamp);

//! Callback type for accelerometer tap events
//! @param axis the axis on which a tap was registered (x, y, or z)
//! @param direction the direction (-1 or +1) of the tap
typedef void (*AccelTapHandler)(AccelAxisType axis, int32_t direction);

//! Valid accelerometer sampling rates, in Hz
typedef enum {
  //! 10 HZ sampling rate
  ACCEL_SAMPLING_10HZ = 10,
  //! 25 HZ sampling rate [Default]
  ACCEL_SAMPLING_25HZ = 25,
  //! 50 HZ sampling rate
  ACCEL_SAMPLING_50HZ = 50,
  //! 100 HZ sampling rate
  ACCEL_SAMPLING_100HZ = 100,
} AccelSamplingRate;

//! Peek at the last recorded reading.
//! @param[out] data a pointer to a pre-allocated AccelData item
//! @note Cannot be used when subscribed to accelerometer data events.
//! @return -1 if the accel is not running
//! @return -2 if subscribed to accelerometer events.
int accel_service_peek(AccelData *data);

//! Change the accelerometer sampling rate.
//! @param rate The sampling rate in Hz (10Hz, 25Hz, 50Hz, and 100Hz possible)
int accel_service_set_sampling_rate(AccelSamplingRate rate);

//! Change the number of samples buffered between each accelerometer data event
//! @param num_samples the number of samples to buffer, between 0 and 25.
int accel_service_set_samples_per_update(uint32_t num_samples);

//! Subscribe to the accelerometer data event service. Once subscribed, the handler
//! gets called every time there are new accelerometer samples available.
//! @note Cannot use \ref accel_service_peek() when subscribed to accelerometer data events.
//! @param handler A callback to be executed on accelerometer data events
//! @param samples_per_update the number of samples to buffer, between 0 and 25.
void accel_data_service_subscribe(uint32_t samples_per_update, AccelDataHandler handler);

//! Unsubscribe from the accelerometer data event service. Once unsubscribed,
//! the previously registered handler will no longer be called.
void accel_data_service_unsubscribe(void);

//! Subscribe to the accelerometer tap event service. Once subscribed, the handler
//! gets called on every tap event emitted by the accelerometer.
//! @param handler A callback to be executed on tap event
void accel_tap_service_subscribe(AccelTapHandler handler);

//! Unsubscribe from the accelerometer tap event service. Once unsubscribed,
//! the previously registered handler will no longer be called.
void accel_tap_service_unsubscribe(void);

//! Subscribe to the accelerometer raw data event service. Once subscribed, the handler
//! gets called every time there are new accelerometer samples available.
//! @note Cannot use \ref accel_service_peek() when subscribed to accelerometer data events.
//! @param handler A callback to be executed on accelerometer data events
//! @param samples_per_update the number of samples to buffer, between 0 and 25.
void accel_raw_data_service_subscribe(uint32_t samples_per_update, AccelRawDataHandler handler);

//! @} // group AccelerometerService

//! @addtogroup CompassService
//!
//! \brief The Compass Service combines information from Pebble's accelerometer and magnetometer to automatically calibrate
//! the compass and transform the raw magnetic field information into a \ref CompassHeading, that is an angle to north.
//!
//! The Compass Service provides magnetic north and information about its status and accuracy through the \ref
//! CompassHeadingData structure. The API is designed to also provide true north in a future release.
//!
//! #### Calibration
//! The Compass Service requires an initial calibration before it can return accurate results. Calibration is
//! performed automatically by the system when required.
//! The `compass_status` field indicates whether the Compass Service is calibrating.
//! To help the calibration process, your application should
//! show a message to the user asking them to move their wrists in different directions.
//!
//! Refer to the compass examples for suggestions on how to implement this screen.
//!
//! #### Magnetic North and True North
//!
//! Depending on your location on earth, the measured heading towards magnetic north and
//! true north can significantly differ. This is called magnetic variation or declination.
//!
//! Pebble does not currently automatically correct the magnetic heading to return a true heading, but the API is designed so that
//! this feature can be added in the future and your applications will be able to automatically take advantage of it:
//!
//!  * If you need a precise heading in your application, we recommend that you use the `magnetic_heading` field and
//! use a webservice to retrieve the declination at your current location.
//!
//!  * If you choose to not correct the heading yourself, we recommend that you use the `true_heading` field. This field will contain
//! the magnetic heading if declination is not available, or the true heading if declination is available. The field
//! `is_declination_valid` will be true when declination is available. You can use this information to tell the user whether
//! you are showing magnetic north or true north.
//!
//! #### Battery Considerations
//! Using the compass will turn on both Pebble's magnetometer and accelerometer. Those two devices will have a slight
//! impact on battery life. A much more significant battery impact will be caused by redrawing the screen too often or
//! performing CPU-intensive work every time the compass heading is updated.
//!
//! We recommend that you follow the following best practices to optimize for battery life:
//!
//! * If your application is already rapidly redrawing the display, use \ref compass_service_peek() to get the most recent
//! available heading when drawing each frame.
//! * If your UI updates only when the heading changes, use \ref compass_service_subscribe() and set an appropriate filter to reduce
//! the number of events.
//! * If you use \ref compass_service_subscribe() and then do not require the compass heading anymore, remember to call
//! \ref compass_service_unsubscribe() to stop the Compass Service.
//! * Finally, note that every time you use \ref compass_service_peek(), the Compass Service will turn on and stay
//! active for a few seconds.
//!
//! #### Defining "up" on Pebble
//! Compass readings are always relative to the current orientation of Pebble. Using the accelerometer, the
//! Compass Service figures out what is the direction that the user is pointing at.
//!
//! If Pebble is held flat, the compass heading will be the angle between a vector to north and a vector going
//! from the bottom to the top of Pebble in a plane parallel to the screen (the Y axis vector of the accelerometer).
//! If the user lifts their arm so that Pebble is held vertical in front of them, compass heading will be relative to a vector
//! going through Pebble (opposite to the Z axis vector of the accelerometer). If the user keeps bringing their arm up,
//! effectively holding the Pebble upside down, the compass heading will be relative to a line from the top to the
//! bottom of Pebble, in the plane of the screen (opposite to the Y axis vector of the accelerometer).
//!
//! #### Code Samples
//! For available code samples, see `Examples/watchapps/feature_compass`.
//! @{

//! Converts from a fixed point value representation of trig_angle to the equivalent value in degrees
#define TRIGANGLE_TO_DEG(trig_angle) (((trig_angle) * 360) / TRIG_MAX_ANGLE)


typedef struct __attribute__((__packed__)) {
 //! magnetic field along the x axis
 int16_t x;
 //! magnetic field along the y axis
 int16_t y;
 //! magnetic field along the z axis
 int16_t z;
} MagData;

//! Enum describing the current state of the Compass Service calibration
typedef enum {
  //! Compass is calibrating: data is invalid and should not be used
  CompassStatusDataInvalid = 0,
  //! Compass is calibrating: the data is valid but the calibration is still being refined
  CompassStatusCalibrating,
  //! Compass data is valid and the calibration has completed
  CompassStatusCalibrated
} CompassStatus;

//! Represents an angle relative to a reference direction, e.g. (magnetic) north.
//! The angle value is scaled linearly, such that a value of TRIG_MAX_ANGLE corresponds to 360 degrees or 2 PI radians.
//! Thus, if heading towards north, north is 0, east is TRIG_MAX_ANGLE/4, south is TRIG_MAX_ANGLE/2, and so on.
typedef int32_t CompassHeading;

//! Structure containing a single heading towards magnetic and true north.
typedef struct {
  //! measured angle relative to magnetic north
  CompassHeading magnetic_heading;
  //! measured angle relative to true north (or to magnetic north if declination
  //! is invalid)
  CompassHeading true_heading;
  //! indicates the current state of the Compass Service calibration
  CompassStatus compass_status;
  //! true, if the current declination is known and applied to `true_heading`, false otherwise
  bool is_declination_valid;
} CompassHeadingData;

//! Callback type for compass heading events
//! @param heading copy of last recorded heading
typedef void (*CompassHeadingHandler)(CompassHeadingData heading);

//! Set the minimum angular change required to generate new compass heading events.
//! The angular distance is measured relative to the last delivered heading event.
//! Use 0 to be notified of all movements.
//! Negative values and values > TRIG_MAX_ANGLE / 2 are not valid.
//! The default value of this property is TRIG_MAX_ANGLE / 360.
//! @return 0, success.
//! @return Non-Zero, if filter is invalid.
//! @see compass_service_subscribe
int compass_service_set_heading_filter(CompassHeading filter);

//! Subscribe to the compass heading event service. Once subscribed, the handler
//! gets called every time the angular distance relative to the previous value
//! exceeds the configured filter.
//! @param handler A callback to be executed on heading events
//! @see compass_service_set_heading_filter
//! @see compass_service_unsubscribe
void compass_service_subscribe(CompassHeadingHandler handler);

//! Unsubscribe from the compass heading event service. Once unsubscribed,
//! the previously registered handler will no longer be called.
//! @see compass_service_subscribe
void compass_service_unsubscribe(void);

//! Peek at the last recorded reading.
//! @param[out] data a pointer to a pre-allocated CompassHeadingData
//! @return Always returns 0 to indicate success.
int compass_service_peek(CompassHeadingData *data);

//! @} // group CompassService

//! @addtogroup TickTimerService
//! \brief Handling time components
//!
//! The TickTimerService allows your app to be called every time one Time component has changed.
//! This is extremely important for watchfaces. Your app can choose on which time component
//! change a tick should occur. Time components are defined by a \ref TimeUnits enum bitmask.
//! @{

//! Time unit flags that can be used to create a bitmask for use in \ref tick_timer_service_subscribe().
//! This will also be passed to \ref TickHandler.
typedef enum {
  //! Flag to represent the "seconds" time unit
  SECOND_UNIT = 1 << 0,
  //! Flag to represent the "minutes" time unit
  MINUTE_UNIT = 1 << 1,
  //! Flag to represent the "hours" time unit
  HOUR_UNIT = 1 << 2,
  //! Flag to represent the "days" time unit
  DAY_UNIT = 1 << 3,
  //! Flag to represent the "months" time unit
  MONTH_UNIT = 1 << 4,
  //! Flag to represent the "years" time unit
  YEAR_UNIT = 1 << 5
} TimeUnits;

//! Callback type for tick timer events
//! @param tick_time the time at which the tick event was triggered
//! @param units_changed which unit change triggered this tick event
typedef void (*TickHandler)(struct tm *tick_time, TimeUnits units_changed);

//! Subscribe to the tick timer event service. Once subscribed, the handler gets called
//! on every requested unit change.
//! Calling this function multiple times will override the units and handler (i.e., only 
//! the last tick_units and handler passed will be used).
//! @param handler The callback to be executed on tick events
//! @param tick_units a bitmask of all the units that have changed
void tick_timer_service_subscribe(TimeUnits tick_units, TickHandler handler);

//! Unsubscribe from the tick timer event service. Once unsubscribed, the previously registered
//! handler will no longer be called.
void tick_timer_service_unsubscribe(void);

//! @} // group TickTimerService

//! @} // group EventService

//! @addtogroup DataLogging
//! \brief Enables logging data asynchronously to a mobile app
//!
//! In Pebble OS, data logging is a data storage and transfer subsystem that allows watchapps to save
//! data on non-volatile storage devices when the phone is not available to process it. The API provides
//! your watchapp with a mechanism for short-term data buffering for asynchronous data transmission to
//! a mobile app.
//!
//! Using this API, your Pebble watchapp can create an arbitrary number of logs, but you’re limited in
//! the amount of storage space you can use. Note that approximately 640K is available for data
//! logging, which is shared among all watchapps that use it. This value is subject to change in the 
//! future. When the data spool is full, an app will start overwriting its own data. An app cannot 
//! overwrite another apps's data. However, the other app might have 0 bytes for data logging.
//!
//! Your app can log data to a session, either creating, adding or deleting data to that session.
//! The data is then sent to the associated phone application at the earliest convenience.
//! If a phone is available, the data is sent directly to the phone. Otherwise, it is saved to the
//! watch storage until the watch is connected to a phone.
//!
//!
//! For example:
//!
//! To create a data logging session for 4-byte unsigned integers with a tag of 0x1234, you would do this:
//! \code{.c}
//!
//! DataLoggingSessionRef logging_session = data_logging_create(0x1234, DATA_LOGGING_UINT, 4, false);
//!
//! // Fake creating some data and logging it to the session.
//! uint32_t data[] = { 1, 2, 3, 4 };
//! data_logging_log(logging_session, &data, 4);
//!
//! // Fake creating more data and logging that as well.
//! uint32_t data2[] = { 1, 2 };
//! data_logging_log(logging_session, &data, 2);
//!
//! // When we don't need to log anything else, we can close off the session.
//! data_logging_finish(logging_session);
//! \endcode
//!
//! For code samples, refer to Examples/data-logging-demo.
//! @{

typedef void *DataLoggingSessionRef;

//! The different types of session data that Pebble supports. This type describes the type of a singular item in the data
//! session. Every item in a given session is the same type and size.
typedef enum {
  //! Array of bytes. Remember that this is the type of a single item in the logging session, so using this
  //! type means you'll be logging multiple byte arrays (each a fixed length described by item_length) for the
  //! duration of the session.
  DATA_LOGGING_BYTE_ARRAY = 0,
  //! Unsigned integer. This may be a 1, 2, or 4 byte integer depending on the item_length parameter
  DATA_LOGGING_UINT = 2,
  //! Signed integer. This may be a 1, 2, or 4 byte integer depending on the item_length parameter
  DATA_LOGGING_INT = 3,
} DataLoggingItemType;

typedef enum {
  DATA_LOGGING_SUCCESS = 0, //!< Successful operation
  DATA_LOGGING_BUSY, //!< Someone else is writing to this logging session
  DATA_LOGGING_FULL, //!< No more space to save data
  DATA_LOGGING_NOT_FOUND, //!< The logging session does not exist
  DATA_LOGGING_CLOSED, //!< The logging session was made inactive
  DATA_LOGGING_INVALID_PARAMS //!< An invalid parameter was passed to one of the functions
} DataLoggingResult;

//! Create a new data logging session.
//!
//! @param tag A tag associated with the logging session.
//! @param item_type The type of data stored in this logging session
//! @param item_length The size of a single data item in bytes
//! @param resume True if we want to look for a logging session of the same tag and
//!   resume logging to it. If this is false and a session with the specified tag exists, that session will
//!   be closed and a new session will be opened.
//! @return An opaque reference to the data logging session
DataLoggingSessionRef
data_logging_create(uint32_t tag, DataLoggingItemType item_type, uint16_t item_length, bool resume);

//! Delete a data logging_session. Logging data is kept until it has successfully been transferred over to the
//! phone, but no data may be added to the session after this function is called.
//!
//! @param logging_session a reference to the data logging session previously allocated using data_logging_create
void data_logging_finish(DataLoggingSessionRef logging_session);

//! Add data to the data logging session. If a phone is available, the data is sent directly
//! to the phone. Otherwise, it is saved to the watch storage until the watch is connected to a phone.
//!
//! @param logging_session a reference to the data logging session you want to add the data to
//! @param data a pointer to the data buffer that contains multiple items
//! @param num_items the number of items to log. This means data must be at least (num_items * item_length)
//!                  long in bytes
//! @return DATA_LOGGING_SUCCESS on success
//!         DATA_LOGGING_NOT_FOUND if the logging session is invalid
//!         DATA_LOGGING_CLOSED if the sesion is not active
//!         DATA_LOGGING_BUSY if the sesion is not available for writing
//!         DATA_LOGGING_INVALID_PARAMS if num_items is 0 or data is NULL
DataLoggingResult
data_logging_log(DataLoggingSessionRef logging_session, const void *data, uint32_t num_items);

//! @} // group DataLogging

//! @addtogroup DataStructures
//! @{

//! @addtogroup UUID
//! @{

typedef struct __attribute__((__packed__)) {
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
} Uuid;

#define UUID_SIZE 16

#define UuidMake(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) ((Uuid) {p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15})

//! Creates a Uuid from an array of bytes with 16 bytes in Big Endian order.
//! @return The created Uuid
#define UuidMakeFromBEBytes(b) ((Uuid) { b[0], b[1], b[2], b[3], \
                                         b[4], b[5], b[6], b[7], \
                                         b[8], b[9], b[10], b[11], \
                                         b[12], b[13], b[14], b[15] })

//! Creates a Uuid from an array of bytes with 16 bytes in Little Endian order.
//! @return The created Uuid
#define UuidMakeFromLEBytes(b) ((Uuid) { b[15], b[14], b[13], b[12], \
                                         b[11], b[10], b[9], b[8], \
                                         b[7], b[6], b[5], b[4], \
                                         b[3], b[2], b[1], b[0] })

//! Compares two UUIDs.
//! @return True if the two UUIDs are equal, false if they are not.
bool uuid_equal(const Uuid *uu1, const Uuid *uu2);

//! Writes UUID in a string form into buffer that looks like the following...
//! {12345678-1234-5678-1234-567812345678}
//! @param uuid The Uuid to write
//! @param buffer Memory to write the string to. Must be at least \ref UUID_STRING_BUFFER_LENGTH bytes long.
void uuid_to_string(const Uuid *uuid, char *buffer);

//! The minimum required length of a string used to hold a uuid (including null).
#define UUID_STRING_BUFFER_LENGTH (32 + 4 + 2 + 1)

//! @} // group UUID

//! @} // group DataStructures

//! @addtogroup Logging Logging
//!   \brief Functions related to logging from apps.
//!
//! This module contains the functions necessary to log messages through
//! Bluetooth.
//! @note It is no longer necessary to enable app logging output from the "settings->about" menu on the Pebble for
//! them to be transmitted!  Instead use the "pebble logs" command included with the SDK to activate logs.  The logs
//! will appear right in your console. Logging
//! over Bluetooth is a fairly power hungry operation that non-developers will
//! not need when your apps are distributed.
//! @{

//! Log an app message.
//! @param log_level
//! @param src_filename The source file where the log originates from
//! @param src_line_number The line number in the source file where the log originates from
//! @param fmt A C formatting string
//! @param ... The arguments for the formatting string
//! @param log_level
//! \sa snprintf for details about the C formatting string.
void app_log(uint8_t log_level, const char* src_filename, int src_line_number, const char* fmt, ...)
    __attribute__((format(printf, 4, 5)));

//! A helper macro that simplifies the use of the app_log function
//! @param level The log level to log output as
//! @param fmt A C formatting string
//! @param args The arguments for the formatting string
#define APP_LOG(level, fmt, args...)                                \
  app_log(level, __FILE_NAME__, __LINE__, fmt, ## args)

//! Suggested log level values
typedef enum {
  //! Error level log message
  APP_LOG_LEVEL_ERROR = 1,
  //! Warning level log message
  APP_LOG_LEVEL_WARNING = 50,
  //! Info level log message
  APP_LOG_LEVEL_INFO = 100,
  //! Debug level log message
  APP_LOG_LEVEL_DEBUG = 200,
  //! Verbose Debug level log message
  APP_LOG_LEVEL_DEBUG_VERBOSE = 255,
} AppLogLevel;

//! @} // group Logging

//! @addtogroup Dictionary
//! \brief Data serialization utilities
//!
//!
//! Data residing in different parts of Pebble memory (RAM) may need to be gathered and assembled into
//! a single continuous block for transport over the network via Bluetooth. The process of gathering
//! and assembling this continuous block of data is called serialization.
//!
//! You use data serialization utilities, like Dictionary, Tuple and Tuplet data structures and accompanying
//! functions, to accomplish this task. No transformations are performed on the actual data, however.
//! These Pebble utilities simply help assemble the data into one continuous buffer according to a
//! specific format.
//!
//! \ref AppMessage uses these utilities--in particular, Dictionary--to send information between mobile
//! and Pebble watchapps.
//!
//! <h3>Writing key/value pairs</h3>
//! To write two key/value pairs, without using Tuplets, you would do this:
//! \code{.c}
//! // Byte array + key:
//! static const uint32_t SOME_DATA_KEY = 0xb00bf00b;
//! static const uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//!
//! // CString + key:
//! static const uint32_t SOME_STRING_KEY = 0xabbababe;
//! static const char *string = "Hello World";
//!
//! // Calculate the buffer size that is needed for the final Dictionary:
//! const uint8_t key_count = 2;
//! const uint32_t size = dict_calc_buffer_size(key_count, sizeof(data),
//!                                             strlen(string) + 1);
//!
//! // Stack-allocated buffer in which to create the Dictionary:
//! uint8_t buffer[size];
//!
//! // Iterator variable, keeps the state of the creation serialization process:
//! DictionaryIterator iter;
//!
//! // Begin:
//! dict_write_begin(&iter, buffer, sizeof(buffer));
//! // Write the Data:
//! dict_write_data(&iter, SOME_DATA_KEY, data, sizeof(data));
//! // Write the CString:
//! dict_write_cstring(&iter, SOME_STRING_KEY, string);
//! // End:
//! const uint32_t final_size = dict_write_end(&iter);
//!
//! // buffer now contains the serialized information
//!
//! \endcode
//!
//! <h3>Reading key/value pairs</h3>
//! To iterate over the key/value pairs in the dictionary that
//! was created in the previous example code, you would do this:
//!
//! \code{.c}
//! Tuple *tuple = dict_read_begin_from_buffer(&iter, buffer, final_size);
//! while (tuple) {
//!   switch (tuple->key) {
//!     case SOME_DATA_KEY:
//!       foo(tuple->value->data, tuple->length);
//!       break;
//!     case SOME_STRING_KEY:
//!       bar(tuple->value->cstring);
//!       break;
//!   }
//!   tuple = dict_read_next(&iter);
//! }
//! \endcode
//!
//! <h3>Tuple and Tuplet data structures</h3>
//! To understand the difference between Tuple and Tuplet data structures:
//! Tuple is the header for a serialized key/value pair, while Tuplet is a helper
//! data structure that references the value you want to serialize. This data
//! structure exists to make the creation of a Dictionary easier to write.
//! Use this mnemonic to remember the difference: TupleT(emplate), the Tuplet being
//! a template to create a Dictionary with Tuple structures.
//!
//! For example:
//! \code{.c}
//! Tuplet pairs[] = {
//!   TupletInteger(WEATHER_ICON_KEY, (uint8_t) 1),
//!   TupletCString(WEATHER_TEMPERATURE_KEY, "1234 Fahrenheit"),
//! };
//! uint8_t buffer[256];
//! uint32_t size = sizeof(buffer);
//! dict_serialize_tuplets_to_buffer(pairs, ARRAY_LENGTH(pairs), buffer, &size);
//!
//! // buffer now contains the serialized information
//! \endcode
//! @{

//! Return values for dictionary write/conversion functions.
typedef enum {
  //! The operation returned successfully
  DICT_OK = 0,
  //! There was not enough backing storage to complete the operation
  DICT_NOT_ENOUGH_STORAGE = 1 << 1,
  //! One or more arguments were invalid or uninitialized
  DICT_INVALID_ARGS = 1 << 2,
  //! The lengths and/or count of the dictionary its tuples are inconsistent
  DICT_INTERNAL_INCONSISTENCY = 1 << 3,
  //! A requested operation required additional memory to be allocated, but
  //! the allocation failed, likely due to insufficient remaining heap memory.
  DICT_MALLOC_FAILED = 1 << 4,
} DictionaryResult;

//! Values representing the type of data that the `value` field of a Tuple contains
typedef enum {
  //! The value is an array of bytes
  TUPLE_BYTE_ARRAY = 0,
  //! The value is a zero-terminated, UTF-8 C-string
  TUPLE_CSTRING = 1,
  //! The value is an unsigned integer. The tuple's `.length` field is used to
  //! determine the size of the integer (1, 2, or 4 bytes).
  TUPLE_UINT = 2,
  //! The value is a signed integer. The tuple's `.length` field is used to
  //! determine the size of the integer (1, 2, or 4 bytes).
  TUPLE_INT = 3,
} TupleType;

//! Data structure for one serialized key/value tuple
//! @note The structure is variable length! The length depends on the value data that the tuple
//! contains.
typedef struct __attribute__((__packed__)) {
  //! The key
  uint32_t key;
  //! The type of data that the `.value` fields contains.
  TupleType type:8;
  //! The length of `.value` in bytes
  uint16_t length;
  //! @brief The value itself.
  //!
  //! The different union fields are provided for convenience, avoiding the need for manual casts.
  //! @note The array length is of incomplete length on purpose, to facilitate
  //! variable length data and because a data length of zero is valid.
  //! @note __Important: The integers are little endian!__
  union {
    //! The byte array value. Valid when `.type` is \ref TUPLE_BYTE_ARRAY.
    uint8_t data[0];
    //! The C-string value. Valid when `.type` is \ref TUPLE_CSTRING.
    char cstring[0];
    //! The 8-bit unsigned integer value. Valid when `.type` is \ref TUPLE_UINT
    //! and `.length` is 1 byte.
    uint8_t uint8;
    //! The 16-bit unsigned integer value. Valid when `.type` is \ref TUPLE_UINT
    //! and `.length` is 2 byte.
    uint16_t uint16;
    //! The 32-bit unsigned integer value. Valid when `.type` is \ref TUPLE_UINT
    //! and `.length` is 4 byte.
    uint32_t uint32;
    //! The 8-bit signed integer value. Valid when `.type` is \ref TUPLE_INT
    //! and `.length` is 1 byte.
    int8_t int8;
    //! The 16-bit signed integer value. Valid when `.type` is \ref TUPLE_INT
    //! and `.length` is 2 byte.
    int16_t int16;
    //! The 32-bit signed integer value. Valid when `.type` is \ref TUPLE_INT
    //! and `.length` is 4 byte.
    int32_t int32;
  } value[];
} Tuple;

struct Dictionary;
typedef struct Dictionary Dictionary;

//! An iterator can be used to iterate over the key/value
//! tuples in an existing dictionary, using \ref dict_read_begin_from_buffer(),
//! \ref dict_read_first() and \ref dict_read_next().
//! An iterator can also be used to append key/value tuples to a dictionary,
//! for example using \ref dict_write_data() or \ref dict_write_cstring().
typedef struct {
  Dictionary *dictionary;  //!< The dictionary being iterated
  const void *end;  //!< Points to the first memory address after the last byte of the dictionary
  //! Points to the next Tuple in the dictionary. Given the end of the
  //! Dictionary has not yet been reached: when writing, the next key/value
  //! pair will be written at the cursor. When reading, the next call
  //! to \ref dict_read_next() will return the cursor.
  Tuple *cursor;
} DictionaryIterator;

//! Calculates the number of bytes that a dictionary will occupy, given
//! one or more value lengths that need to be stored in the dictionary.
//! @note The formula to calculate the size of a Dictionary in bytes is:
//! <pre>1 + (n * 7) + D1 + ... + Dn</pre>
//! Where `n` is the number of Tuples in the Dictionary and `Dx` are the sizes
//! of the values in the Tuples. The size of the Dictionary header is 1 byte.
//! The size of the header for each Tuple is 7 bytes.
//! @param tuple_count The total number of key/value pairs in the dictionary.
//! @param ... The sizes of each of the values that need to be
//! stored in the dictionary.
//! @return The total number of bytes of storage needed.
uint32_t dict_calc_buffer_size(const uint8_t tuple_count, ...);

//! Calculates the size of data that has been written to the dictionary.
//! AKA, the "dictionary size". Note that this is most likely different
//! than the size of the backing storage/backing buffer.
//! @param iter The dictionary iterator
//! @return The total number of bytes which have been written to the dictionary.
uint32_t dict_size(DictionaryIterator* iter);

//! Initializes the dictionary iterator with a given buffer and size,
//! resets and empties it, in preparation of writing key/value tuples.
//! @param iter The dictionary iterator
//! @param buffer The storage of the dictionary
//! @param size The storage size of the dictionary
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
//! @see dict_calc_buffer_size
//! @see dict_write_end
DictionaryResult dict_write_begin(DictionaryIterator *iter, uint8_t * const buffer, const uint16_t size);

//! Adds a key with a byte array value pair to the dictionary.
//! @param iter The dictionary iterator
//! @param key The key
//! @param data Pointer to the byte array
//! @param size Length of the byte array
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
//! @note The data will be copied into the backing storage of the dictionary.
//! @note There is _no_ checking for duplicate keys.
DictionaryResult dict_write_data(DictionaryIterator *iter, const uint32_t key, const uint8_t * const data, const uint16_t size);

//! Adds a key with a C string value pair to the dictionary.
//! @param iter The dictionary iterator
//! @param key The key
//! @param cstring Pointer to the zero-terminated C string
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
//! @note The string will be copied into the backing storage of the dictionary.
//! @note There is _no_ checking for duplicate keys.
DictionaryResult dict_write_cstring(DictionaryIterator *iter, const uint32_t key, const char * const cstring);

//! Adds a key with an integer value pair to the dictionary.
//! @param iter The dictionary iterator
//! @param key The key
//! @param integer Pointer to the integer value
//! @param width_bytes The width of the integer value
//! @param is_signed Whether the integer's type is signed or not
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
//! @note There is _no_ checking for duplicate keys. dict_write_int() is only for serializing a single
//! integer. width_bytes can only be 1, 2, or 4.
DictionaryResult dict_write_int(DictionaryIterator *iter, const uint32_t key, const void *integer, const uint8_t width_bytes, const bool is_signed);

//! Adds a key with an unsigned, 8-bit integer value pair to the dictionary.
//! @param iter The dictionary iterator
//! @param key The key
//! @param value The unsigned, 8-bit integer value
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
//! @note There is _no_ checking for duplicate keys.
//! @note There are counterpart functions for different signedness and widths,
//! `dict_write_uint16()`, `dict_write_uint32()`, `dict_write_int8()`,
//! `dict_write_int16()` and `dict_write_int32()`. The documentation is not
//! repeated for brevity's sake.
DictionaryResult dict_write_uint8(DictionaryIterator *iter, const uint32_t key, const uint8_t value);

DictionaryResult dict_write_uint16(DictionaryIterator *iter, const uint32_t key, const uint16_t value);

DictionaryResult dict_write_uint32(DictionaryIterator *iter, const uint32_t key, const uint32_t value);

DictionaryResult dict_write_int8(DictionaryIterator *iter, const uint32_t key, const int8_t value);

DictionaryResult dict_write_int16(DictionaryIterator *iter, const uint32_t key, const int16_t value);

DictionaryResult dict_write_int32(DictionaryIterator *iter, const uint32_t key, const int32_t value);

//! End a series of writing operations to a dictionary.
//! This must be called before reading back from the dictionary.
//! @param iter The dictionary iterator
//! @return The size in bytes of the finalized dictionary, or 0 if the parameters were invalid.
uint32_t dict_write_end(DictionaryIterator *iter);

//! Initializes the dictionary iterator with a given buffer and size,
//! in preparation of reading key/value tuples.
//! @param iter The dictionary iterator
//! @param buffer The storage of the dictionary
//! @param size The storage size of the dictionary
//! @return The first tuple in the dictionary, or NULL in case the dictionary was empty or if there was a parsing error.
Tuple * dict_read_begin_from_buffer(DictionaryIterator *iter, const uint8_t * const buffer, const uint16_t size);

//! Progresses the iterator to the next key/value pair.
//! @param iter The dictionary iterator
//! @return The next tuple in the dictionary, or NULL in case the end has been reached or if there was a parsing error.
Tuple * dict_read_next(DictionaryIterator *iter);

//! Resets the iterator back to the same state as a call to \ref dict_read_begin_from_buffer() would do.
//! @param iter The dictionary iterator
//! @return The first tuple in the dictionary, or NULL in case the dictionary was empty or if there was a parsing error.
Tuple * dict_read_first(DictionaryIterator *iter);

//! Non-serialized, template data structure for a key/value pair.
//! For strings and byte arrays, it only has a pointer to the actual data.
//! For integers, it provides storage for integers up to 32-bits wide.
//! The Tuplet data structure is useful when creating dictionaries from values
//! that are already stored in arbitrary buffers.
//! See also \ref Tuple, with is the header of a serialized key/value pair.
typedef struct Tuplet {
  //! The type of the Tuplet. This determines which of the struct fields in the
  //! anonymomous union are valid.
  TupleType type;
  //! The key.
  uint32_t key;
  //! Anonymous union containing the reference to the Tuplet's value, being
  //! either a byte array, c-string or integer. See documentation of `.bytes`,
  //! `.cstring` and `.integer` fields.
  union {
    //! Valid when `.type.` is \ref TUPLE_BYTE_ARRAY
    struct {
      //! Pointer to the data
      const uint8_t *data;
      //! Length of the data
      const uint16_t length;
    } bytes;
    //! Valid when `.type.` is \ref TUPLE_CSTRING
    struct {
      //! Pointer to the c-string data
      const char *data;
      //! Length of the c-string, including terminating zero.
      const uint16_t length;
    } cstring;
    //! Valid when `.type.` is \ref TUPLE_INT or \ref TUPLE_UINT
    struct {
      //! Actual storage of the integer.
      //! The signedness can be derived from the `.type` value.
      uint32_t storage;
      //! Width of the integer.
      const uint16_t width;
    } integer;
  }; //!< See documentation of `.bytes`, `.cstring` and `.integer` fields.
} Tuplet;

//! Macro to create a Tuplet with a byte array value
//! @param _key The key
//! @param _data Pointer to the bytes
//! @param _length Length of the buffer
#define TupletBytes(_key, _data, _length) \
((const Tuplet) { .type = TUPLE_BYTE_ARRAY, .key = _key, .bytes = { .data = _data, .length = _length }})

//! Macro to create a Tuplet with a c-string value
//! @param _key The key
//! @param _cstring The c-string value
#define TupletCString(_key, _cstring) \
((const Tuplet) { .type = TUPLE_CSTRING, .key = _key, .cstring = { .data = _cstring, .length = _cstring ? strlen(_cstring) + 1 : 0 }})

//! Macro to create a Tuplet with an integer value
//! @param _key The key
//! @param _integer The integer value
#define TupletInteger(_key, _integer) \
((const Tuplet) { .type = IS_SIGNED(_integer) ? TUPLE_INT : TUPLE_UINT, .key = _key, .integer = { .storage = _integer, .width = sizeof(_integer) }})

//! Callback for \ref dict_serialize_tuplets() utility.
//! @param data The data of the serialized dictionary
//! @param size The size of data
//! @param context The context pointer as passed in to \ref dict_serialize_tuplets()
//! @see dict_serialize_tuplets
typedef void (*DictionarySerializeCallback)(const uint8_t * const data, const uint16_t size, void *context);

//! Utility function that takes a list of Tuplets from which a dictionary
//! will be serialized, ready to transmit or store.
//! @note The callback will be called before the function returns, so the data that
//! that `context` points to, can be stack allocated.
//! @param callback The callback that will be called with the serialized data of the generated dictionary.
//! @param context Pointer to any application specific data that gets passed into the callback.
//! @param tuplets An array of Tuplets that need to be serialized into the dictionary.
//! @param tuplets_count The number of tuplets that follow.
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
DictionaryResult dict_serialize_tuplets(DictionarySerializeCallback callback, void *context, const Tuplet * const tuplets, const uint8_t tuplets_count);

//! Utility function that takes an array of Tuplets and serializes them into
//! a dictionary with a given buffer and size.
//! @param tuplets The array of tuplets
//! @param tuplets_count The number of tuplets in the array
//! @param buffer The buffer in which to write the serialized dictionary
//! @param [in] size_in_out The available buffer size in bytes
//! @param [out] size_in_out The number of bytes written
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
DictionaryResult dict_serialize_tuplets_to_buffer(const Tuplet * const tuplets, const uint8_t tuplets_count, uint8_t *buffer, uint32_t *size_in_out);

//! Serializes an array of Tuplets into a dictionary with a given buffer and size.
//! @param iter The dictionary iterator
//! @param tuplets The array of tuplets
//! @param tuplets_count The number of tuplets in the array
//! @param buffer The buffer in which to write the serialized dictionary
//! @param [in] size_in_out The available buffer size in bytes
//! @param [out] size_in_out The number of bytes written
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
DictionaryResult dict_serialize_tuplets_to_buffer_with_iter(DictionaryIterator *iter, const Tuplet * const tuplets, const uint8_t tuplets_count, uint8_t *buffer, uint32_t *size_in_out);

//! Serializes a Tuplet and writes the resulting Tuple into a dictionary.
//! @param iter The dictionary iterator
//! @param tuplet The Tuplet describing the key/value pair to write
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
DictionaryResult dict_write_tuplet(DictionaryIterator *iter, const Tuplet * const tuplet);

//! Calculates the number of bytes that a dictionary will occupy, given
//! one or more Tuplets that need to be stored in the dictionary.
//! @note See \ref dict_calc_buffer_size() for the formula for the calculation.
//! @param tuplets An array of Tuplets that need to be stored in the dictionary.
//! @param tuplets_count The total number of Tuplets that follow.
//! @return The total number of bytes of storage needed.
//! @see Tuplet
uint32_t dict_calc_buffer_size_from_tuplets(const Tuplet * const tuplets, const uint8_t tuplets_count);

//! Type of the callback used in \ref dict_merge()
//! @param key The key that is being updated.
//! @param new_tuple The new tuple. The tuple points to the actual, updated destination dictionary or NULL_TUPLE
//! in case there was an error (e.g. backing buffer was too small).
//! Therefore the Tuple can be used after the callback returns, until the destination dictionary
//! storage is free'd (by the application itself).
//! @param old_tuple The values that will be replaced with `new_tuple`. The key, value and type will be
//! equal to the previous tuple in the old destination dictionary, however the `old_tuple points
//! to a stack-allocated copy of the old data.
//! @param context Pointer to application specific data
//! The storage backing `old_tuple` can only be used during the callback and
//! will no longer be valid after the callback returns.
//! @see dict_merge
typedef void (*DictionaryKeyUpdatedCallback)(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);

//! Merges entries from another "source" dictionary into a "destination" dictionary.
//! All Tuples from the source are written into the destination dictionary, while
//! updating the exsting Tuples with matching keys.
//! @param dest The destination dictionary to update
//! @param [in,out] dest_max_size_in_out In: the maximum size of buffer backing `dest`. Out: the final size of the updated dictionary.
//! @param source The source dictionary of which its Tuples will be used to update dest.
//! @param update_existing_keys_only Specify True if only the existing keys in `dest` should be updated.
//! @param key_callback The callback that will be called for each Tuple in the merged destination dictionary.
//! @param context Pointer to app specific data that will get passed in when `update_key_callback` is called.
//! @return \ref DICT_OK, \ref DICT_INVALID_ARGS, \ref DICT_NOT_ENOUGH_STORAGE
DictionaryResult dict_merge(DictionaryIterator *dest, uint32_t *dest_max_size_in_out,
                             DictionaryIterator *source,
                             const bool update_existing_keys_only,
                             const DictionaryKeyUpdatedCallback key_callback, void *context);

//! Tries to find a Tuple with specified key in a dictionary
//! @param iter Iterator to the dictionary to search in.
//! @param key The key for which to find a Tuple
//! @return Pointer to a found Tuple, or NULL if there was no Tuple with the specified key.
Tuple *dict_find(const DictionaryIterator *iter, const uint32_t key);

//! @} // group Dictionary

//! @addtogroup Worker
//! @{

//! The event loop for workers, to be used in worker's main(). Will block until the worker is ready to exit.
//! @see \ref App
void worker_event_loop(void);

//! Launch the foreground app for this worker
void worker_launch_app(void);

//! @} // group Worker

//! @addtogroup AppWorker
//!
//! \brief Managing the app worker
//!
//! This modules contains functions for and managing the worker task, querying its status, and communicating with it
//!
//! @{

//! Possible error codes from app_worker_launch, app_worker_kill
typedef enum {
  //! Success
  APP_WORKER_RESULT_SUCCESS= 0,
  //! No worker found for the current app
  APP_WORKER_RESULT_NO_WORKER = 1,
  //! A worker for a different app is already running
  APP_WORKER_RESULT_DIFFERENT_APP = 2,
  //! The worker is not running
  APP_WORKER_RESULT_NOT_RUNNING = 3,
  //! The worker is already running
  APP_WORKER_RESULT_ALREADY_RUNNING = 4,
  //! The user will be asked for confirmation
  APP_WORKER_RESULT_ASKING_CONFIRMATION = 5,
} AppWorkerResult;

//! Generic structure of a worker message that can be sent between an app and its worker
typedef struct {
  uint16_t data0;
  uint16_t data1;
  uint16_t data2;
} AppWorkerMessage;

//! Determine if the worker for the current app is running
//! @return true if running
bool app_worker_is_running(void);

//! Launch the worker for the current app. Note that this is an asynchronous operation, a result code
//! of APP_WORKER_RESULT_SUCCESS merely means that the request was successfully queued up.
//! @return result code
AppWorkerResult app_worker_launch(void);

//! Kill the worker for the current app. Note that this is an asynchronous operation, a result code
//! of APP_WORKER_RESULT_SUCCESS merely means that the request was successfully queued up.
//! @return result code
AppWorkerResult app_worker_kill(void);

//! Callback type for worker messages. Messages can be sent from worker to app or vice versa.
//! @param type An application defined message type
//! @param data pointer to message data. The receiver must know the structure of the data provided by the sender.
typedef void (*AppWorkerMessageHandler)(uint16_t type, AppWorkerMessage *data);

//! Subscribe to worker messages. Once subscribed, the handler gets called on every message emitted by the other task
//! (either worker or app).
//! @param handler A callback to be executed when the event is received
//! @return true on success
bool app_worker_message_subscribe(AppWorkerMessageHandler handler);

//! Unsubscribe from worker messages. Once unsubscribed, the previously registered handler will no longer be called.
//! @return true on success
bool app_worker_message_unsubscribe(void);

//! Send a message to the other task (either worker or app).
//! @param type An application defined message type
//! @param data the message data structure
void app_worker_send_message(uint8_t type, AppWorkerMessage *data);

//! @} // group AppWorker

//! @addtogroup Timer
//! @{

//! Waits for a certain amount of milliseconds
//! @param millis The number of milliseconds to wait for
void psleep(int millis);

struct AppTimer;
typedef struct AppTimer AppTimer;

//! The type of function which can be called when a timer fires.  The argument will be the @p callback_data passed to
//! @ref app_timer_register().
typedef void (*AppTimerCallback)(void* data);

//! Registers a timer that ends up in callback being called some specified time in the future.
//! @param timeout_ms The expiry time in milliseconds from the current time
//! @param callback The callback that gets called at expiry time
//! @param callback_data The data that will be passed to callback
AppTimer* app_timer_register(uint32_t timeout_ms, AppTimerCallback callback, void* callback_data);

//! Reschedules an already running timer for some point in the future.
//! @param timer_handle The timer to reschedule
//! @param new_timeout_ms The new expiry time in milliseconds from the current time
//! @return true if the timer was rescheduled, false if the timer has already elapsed
bool app_timer_reschedule(AppTimer *timer_handle, uint32_t new_timeout_ms);

//! Cancels an already registered timer. Once cancelled the the handle may longer be used for any purpose.
void app_timer_cancel(AppTimer *timer_handle);

//! @} // group Timer

//! @addtogroup MemoryManagement Memory Management
//!   \brief Utility functions for determining an application's memory usage.
//!
//! @{

//! Calculates the number of bytes of heap memory \a not currently being used by the application.
//! @return The number of bytes on the heap not currently being used.
size_t heap_bytes_free(void);

//! Calculates the number of bytes of heap memory currently being used by the application.
//! @return The number of bytes on the heap currently being used.
size_t heap_bytes_used(void);

//! @} // group MemoryManagement

//! @addtogroup Storage
//! \brief A mechanism to store persistent application data and state
//!
//! The Persistent Storage API provides you with a mechanism for performing a variety of tasks,
//! like saving user settings, caching data from the phone app, or counting high scores for
//! Pebble watchapp games.
//!
//! In Pebble OS, storage is defined by a collection of fields that you can create, modify or delete.
//! In the API, a field is specified as a key with a corresponding value.
//!
//! Using the Storage API, every app is able to get its own persistent storage space. Each value
//! in that space is associated with a uint32_t key.
//!
//! Storage supports saving integers, strings and byte arrays. The maximum size of byte arrays and
//! strings is defined by PERSIST_DATA_MAX_LENGTH (currently set to 256 bytes). You call the function
//! persist_exists(key), which returns a boolean indicating if the key exists or not.
//! The Storage API enables your app to save its state, and when compared to using \ref AppMessage to
//! retrieve values from the phone, it provides you with a much faster way to restore state.
//! In addition, it draws less power from the battery.
//!
//! Note that the size of all persisted values cannot exceed 4K.
//! @{

//! The maximum size of a persist value in bytes
#define PERSIST_DATA_MAX_LENGTH 256

//! The maximum size of a persist string in bytes including the NULL terminator
#define PERSIST_STRING_MAX_LENGTH PERSIST_DATA_MAX_LENGTH

//! Status codes. See \ref status_t
typedef enum StatusCode {
  //! Operation completed successfully.
  S_SUCCESS = 0,

  //! An error occurred (no description).
  E_ERROR = -1,

  //! No idea what went wrong.
  E_UNKNOWN = -2,

  //! There was a generic internal logic error.
  E_INTERNAL = -3,

  //! The function was not called correctly.
  E_INVALID_ARGUMENT = -4,

  //! Insufficient allocatable memory available.
  E_OUT_OF_MEMORY = -5,

  //! Insufficient long-term storage available.
  E_OUT_OF_STORAGE = -6,

  //! Insufficient resources available.
  E_OUT_OF_RESOURCES = -7,

  //! Argument out of range (may be dynamic).
  E_RANGE = -8,

  //! Target of operation does not exist.
  E_DOES_NOT_EXIST = -9,

  //! Operation not allowed (may depend on state).
  E_INVALID_OPERATION = -10,

  //! Another operation prevented this one.
  E_BUSY = -11,

  //! Equivalent of boolean true.
  S_TRUE = 1,

  //! Equivalent of boolean false.
  S_FALSE = 0,

  //! For list-style requests.  At end of list.
  S_NO_MORE_ITEMS = 2,

  //! No action was taken as none was required.
  S_NO_ACTION_REQUIRED = 3,

} StatusCode;

//! Return value for system operations. See \ref StatusCode for possible values.
typedef int32_t status_t;

//! Checks whether a value has been set for a given key in persistent storage.
//! @param key The key of the field to check.
//! @return true if a value exists, otherwise false.
bool persist_exists(const uint32_t key);

//! Gets the size of a value for a given key in persistent storage.
//! @param key The key of the field to lookup the data size.
//! @return The size of the value in bytes or \ref E_DOES_NOT_EXIST
//! if there is no field matching the given key.
int persist_get_size(const uint32_t key);

//! Reads a bool value for a given key from persistent storage.
//! If the value has not yet been set, this will return false.
//! @param key The key of the field to read from.
//! @return The bool value of the key to read from.
bool persist_read_bool(const uint32_t key);

//! Reads an int value for a given key from persistent storage.
//! @note The int is a signed 32-bit integer.
//! If the value has not yet been set, this will return 0.
//! @param key The key of the field to read from.
//! @return The int value of the key to read from.
int32_t persist_read_int(const uint32_t key);

//! Reads a blob of data for a given key from persistent storage into a given buffer.
//! If the value has not yet been set, the given buffer is left unchanged.
//! @param key The key of the field to read from.
//! @param buffer The pointer to a buffer to be written to.
//! @param buffer_size The maximum size of the given buffer.
//! @return The number of bytes written into the buffer or \ref E_DOES_NOT_EXIST
//! if there is no field matching the given key.
int persist_read_data(const uint32_t key, void *buffer, const size_t buffer_size);

//! Reads a string for a given key from persistent storage into a given buffer.
//! The string will be null terminated.
//! If the value has not yet been set, the given buffer is left unchanged.
//! @param key The key of the field to read from.
//! @param buffer The pointer to a buffer to be written to.
//! @param buffer_size The maximum size of the given buffer. This includes the null character.
//! @return The number of bytes written into the buffer or \ref E_DOES_NOT_EXIST
//! if there is no field matching the given key.
int persist_read_string(const uint32_t key, char *buffer, const size_t buffer_size);

//! Writes a bool value flag for a given key into persistent storage.
//! @param key The key of the field to write to.
//! @param value The boolean value to write.
status_t persist_write_bool(const uint32_t key, const bool value);

//! Writes an int value for a given key into persistent storage.
//! @note The int is a signed 32-bit integer.
//! @param key The key of the field to write to.
//! @param value The int value to write.
status_t persist_write_int(const uint32_t key, const int32_t value);

//! Writes a blob of data of a specified size in bytes for a given key into persistent storage.
//! The maximum size is \ref PERSIST_DATA_MAX_LENGTH
//! @param key The key of the field to write to.
//! @param data The pointer to the blob of data.
//! @param size The size in bytes.
//! @return The number of bytes written.
int persist_write_data(const uint32_t key, const void *data, const size_t size);

//! Writes a string a given key into persistent storage.
//! The maximum size is \ref PERSIST_STRING_MAX_LENGTH including the null terminator.
//! @param key The key of the field to write to.
//! @param cstring The pointer to null terminated string.
//! @return The number of bytes written.
int persist_write_string(const uint32_t key, const char *cstring);

//! Deletes the value of a key from persistent storage.
//! @param key The key of the field to delete from.
status_t persist_delete(const uint32_t key);

//! @} // group Storage

//! @} // group Foundation

//! Returns the current time in Unix Timestamp Format with Milliseconds
//!     @param tloc if provided receives current Unix Time seconds portion
//!     @param out_ms if provided receives current Unix Time milliseconds portion
//!     @return Current Unix Time milliseconds portion
uint16_t time_ms(time_t *tloc, uint16_t *out_ms);
