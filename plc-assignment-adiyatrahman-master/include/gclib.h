/*! \file gclib.h
*
* Defines the interface for the Galil C Library (GCLIB).
*
*/
#ifndef I_D48432D9_1FA3_4C7D_B44C_05F8B9000ADF
#define I_D48432D9_1FA3_4C7D_B44C_05F8B9000ADF

//set library visibility for gcc and msvc
#if BUILDING_GCLIB && HAVE_VISIBILITY
#define GCLIB_DLL_EXPORTED __attribute__((__visibility__("default")))
#elif BUILDING_GCLIB && defined _MSC_VER
#define GCLIB_DLL_EXPORTED __declspec(dllexport)
#elif defined _MSC_VER
#define GCLIB_DLL_EXPORTED __declspec(dllimport)
#else
#define GCLIB_DLL_EXPORTED
#endif

#include "gclib_record.h" // Galil data record structs and unions.
#include "gclib_errors.h" // GReturn error code values and strings.

#ifdef _WIN32
#define GCALL __stdcall //!< Specify calling convention for Windows.
#else
#define GCALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

	//Constants for function arguments
#define G_DR 1 //!< Value for GRecord() `method` variable for acquiring a data record via DR mode.
#define G_QR 0 //!< Value for GRecord() `method` variable for acquiring a data record via QR mode.
#define G_BOUNDS -1 //!< For functions that take range options, e.g. GArrayUpload(), use this value for full range.
#define G_CR 0 //!< For GArrayUpload(), use this value in the delim field to delimit with carriage returns.
#define G_COMMA 1 //!< For GArrayUpload(), use this value in the delim field to delimit with commas.

	//Constants for GUtility()
#define G_UTIL_TIMEOUT 1 //!< GUtility(), Access to timeout.
#define G_UTIL_TIMEOUT_OVERRIDE 2 //!< GUtility(), read/write access to timeout override.
#define G_USE_INITIAL_TIMEOUT -1 //!< GUtility(), for timeout override. Set `G_UTIL_TIMEOUT_OVERRIDE` to this value to use initial GOpen() timeout (`--timeout`).
#define G_UTIL_VERSION 128 //!< GUtility(), get a library version string.
#define G_UTIL_INFO 129 //!< GUtility(), get a connection info string.
#define G_UTIL_SLEEP 130 //!< GUtility(), specify an interval to sleep.
#define G_UTIL_ADDRESSES 131 //!< GUtility(), get a list of available connections.
#define G_UTIL_IPREQUEST 132 //!< GUtility(), get a list of hardware requesting IPs.
#define G_UTIL_ASSIGN 133 //!< GUtility(), assign.
#define G_UTIL_GCAPS_KEEPALIVE 134 //!< GUtility(), gcaps keepalive.
#define G_UTIL_DEVICE_INITIALIZE 135  //!< GUtility(), sends CF, CW, EO etc. to initialize the connection. Useful after RS or other reset.	
	

	//Convenience constants
#define G_SMALL_BUFFER 1024 //!< Most reads/writes to Galil are small. This value will easily hold most, e.g. TH, TZ, etc.
#define G_HUGE_BUFFER 524288 //!< Most reads/writes to Galil hardware are small. This value will hold the largest array or program upload/download possible.

	typedef int GReturn; //!< Every function returns a value of type GReturn. See gclib_errors.h for possible values.
	typedef void* GCon; //!< Connection handle. Unique for each connection in process. Assigned a non-zero value in GOpen().
	typedef unsigned int GSize; //!< Size of buffers, etc.
	typedef int GOption; //!< Option integer for various formatting, etc.
	typedef char* GCStringOut; //!< C-string output from the library. Implies null-termination.
	typedef const char* GCStringIn; //!< C-string input to the library.  Implies null-termination.
	typedef char* GBufOut; //!< Data output from the library. No null-termination implied. Returned values may be null-terminated, see function documentation for details.
	typedef const char* GBufIn; //!< Data input to the library. No null-termination, function will have a GSize to indicate bytes to write .
	typedef unsigned char GStatus; //!< Interrupt status byte.
	typedef void* GMemory; //!< Pointer to untyped memory for use in GUtility().

	//! Open a connection to a Galil Controller.
	GCLIB_DLL_EXPORTED GReturn GCALL GOpen(GCStringIn address, GCon* g);
	/*!<
	*  \param address Null-terminated address string. See table below.
	*  \param g Pointer to user's `GCon` variable. On success, the library will fill the user's variable with the handle to use for the rest of the connection. A valid `g` value is nonzero.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  `address` switch     | Meaning                                                         | Arguments `(default), other options`  | Examples
	*  ---------------------|-----------------------------------------------------------------|---------------------------------------|-----------------
	*  `--address`          | **Simple address to hardware**                                  | *IP address*, *PCI*, *COM port*       | `--address COM1`
	*  `-a`                 | shorthand for `--address`                                       |  See *Address Ranges* below           | `-a GALILPCI1`
	*  {no switch}          | `--address` is implicit for any lone token                      |                                       | `192.168.0.42`
	*  `--baud`             | **Baud rate**                                                   | (`115200`), *valid baud...*           | `COM2 --baud 19200`
	*  `-b`                 | shorthand for `--baud`                                          |                                       | `COM3 -b 38400`
	*  `--command`          | **Command-and-response socket protocol**                        |  (`TCP`), `UDP`                       | `192.168.0.42 --command TCP`
	*  `-c`                 | shorthand for `--command`                                       |                                       | `192.168.0.42 -c UDP`
	*  `--direct`           | **Connect directly to hardware instead of via** @ref gcaps      |                                       | `-a GALILPCI2 --direct`
	*  `-d`                 | shorthand for `--direct`                                        |                                       | `GALILPCI2 -d`
	*  `--handshake`        | **Serial Handshake mode**                                       | (`HARDWARE`), `NONE`                  | `COM1 --handshake NONE`
	*  `--p1`               | **Primary port for command-and-response traffic**               | (`23`), *valid port number*           | `192.168.0.42 --p1 5000`
	*  `--p2`               | **Secondary port for unsolicited traffic**                      | (`60007`), *valid port number*        | `192.168.0.42 --p2 5000`
	*  `--subscribe`        | **Subscribe to messages, data records, and/or interrupts**      | (`NONE`), `MG`, `DR`, `EI`, `ALL`     | `192.168.0.42 --subscribe MG`
	*  `-s`                 | shorthand for `--subscribe`                                     |                                       | `192.168.0.42 -s DR -s EI`
	*  `--timeout`          | **timeout in ms**                                               | (`5000`), *0-65535*                   | `192.168.0.42 --timeout 5000`
	*  `-t`                 | shorthand for `--timeout`                                       |                                       | `GALILPCI2 -t 500`
	*  `--unsolicited`      | **Unsolicited socket protocol**                                 | (`UDP`), `TCP`, `NONE`                | `192.168.0.42 --unsolicited TCP`
	*  `-u`                 | shorthand for `--unsolicited`                                   |                                       | `192.168.1.42 -u NONE`
	*
	*
	*
	*  Operating System     |  Address Range                     | Notes
	*  ---------------------|------------------------------------|------------------------------
	*  Windows              | `COM1` - `COM256`                  | RS232 and USB-to-serial
	*  Linux                | `/dev/ttyS0` - `/dev/ttyS255`      | RS232
	*  Linux                | `/dev/ttyUSB0` - `/dev/ttyUSB255`  | USB-to-serial, e.g. DMC-4103
	*  Windows              | `GALILPCI1` - `GALILPCI8`          | PCI 
	*  Linux                | `/dev/galilpci0` - `/dev/galilpci7`| PCI 
	*
	*  See x_examples.cpp for an example.
	*
	*  When connecting to a network device, if the command-and-response socket is opened successfully but the unsolicited socket fails, GOpen() will still
	*  complete successfully. This allows connection to a Galil controller when only one Ethernet handle is available. Unsolicited traffic will not be accessible
	*  in this case.
	*/


	//! Closes a connection to a Galil Controller.
	GCLIB_DLL_EXPORTED GReturn GCALL GClose(GCon g);
	/*!<
	* \attention
	*  *gclib* requires that `GClose()` be called whenever a program is finished with a controller.
	*  This includes when a program closes. A rule of thumb is that for every `GOpen()` call on a given connection,
	*  a `GClose()` call should be found on every code path. Failing to call GClose() may cause controller resources
	*  to not be released or can hang the process if there are outstanding asynchronous operations. The latter can
	*  occur, for example, if a call to GRead() times out and the process exits without calling GClose(). In this case, 
	*  GRead() still has an outstanding asynchronous read pending. GClose() will terminate this operation allowing the process 
	*  to exit correctly.
	*
	*  \param g Connection's handle.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  See x_examples.cpp for an example.
	*/


	//! Performs a read on the connection.
	GCLIB_DLL_EXPORTED GReturn GCALL GRead(GCon g, GBufOut buffer, GSize buffer_len, GSize* bytes_read);
	/*!<
	*  \param g Connection's handle.
	*  \param buffer The user's read buffer.
	*  \param buffer_len The length of the user's read buffer.
	*  \param bytes_read Pointer to a GSize which will be filled with the number of bytes read upon return.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  \warning This function is deprecated and will be removed in a future gclib version. Please contact Galil
	*  for needs not covered by the other gclib functions.
	*
	*  Unsolicited messages may be returned in the read data. 
	*  The high bit of each message byte will be set unless the user changes the CW setting.
	*  Interrupts and Data Records are always filtered from a read.
	*
	*  See x_gread_gwrite.cpp for an example.
	*/


	//! Performs a write on the connection.
	GCLIB_DLL_EXPORTED GReturn GCALL GWrite(GCon g, GBufIn buffer, GSize buffer_len);
	/*!<
	*  \param g Connection's handle.
	*  \param buffer The user's write buffer. To send a Galil command, a terminating carriage return is usually required.
	*  \param buffer_len The length of the data in the buffer.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*          If G_NO_ERROR is returned, all bytes were written.
	*
	*  \warning This function is deprecated and will be removed in a future gclib version. Please contact Galil
	*  for needs not covered by the other gclib functions.
	*
	*  See x_gread_gwrite.cpp for an example.
	*/


	//! Performs a *command-and-response* transaction on the connection.
	GCLIB_DLL_EXPORTED GReturn GCALL GCommand(GCon g, GCStringIn command, GBufOut buffer, GSize buffer_len, GSize* bytes_returned);
	/*!<
	*  \param g Connection's handle.
	*  \param command Null-terminated command string to send to the controller. The library will append a carriage return to the command string.
	*  \param buffer Buffer for the response. Will be filled with the response from the controller. The data will be null terminated unless function returns `G_BAD_LOST_DATA` due to the buffer being too small to hold the data.
	*  \param buffer_len The size of the response buffer.
	*  \param bytes_returned The size of the data returned from the controller. This does not include null termination. This argument may be null if the value is not desired.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  See x_gcommand.cpp for an example.
	*/


	//! Downloads a program to the controller's program buffer.
	GCLIB_DLL_EXPORTED GReturn GCALL GProgramDownload(GCon g, GCStringIn program, GCStringIn preprocessor);
	/*!<
	*  \param g Connection's handle.
	*  \param program Null-terminated program for download.
	*  \param preprocessor Options string for preprocessing the program before sending it to the controller.
  *         Null allows the library to use defaults for the download.
  *         See the \ref preprocessor documentation for options.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
  *
	*  See x_programs.cpp for an example.
	*/


	//! Uploads a program from the controller's program buffer.
	GCLIB_DLL_EXPORTED GReturn GCALL GProgramUpload(GCon g, GBufOut buffer, GSize buffer_len);
	/*!<
	*  \param g Connection's handle.
	*  \param buffer Buffer to receive the controller's program. The data will be null terminated unless function returns `G_BAD_LOST_DATA` due to the buffer being too small to hold the data.
	*  \param buffer_len The length of the receive buffer.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  See x_programs.cpp for an example.
	*/


	//! Downloads array data to a pre-dimensioned array in the controller's array table.
	GCLIB_DLL_EXPORTED GReturn GCALL GArrayDownload(GCon g, const GCStringIn array_name, GOption first, GOption last, GCStringIn buffer);
	/*!<
	*  \warning The array must already exist on the controller and be sufficient dimension to hold the desired array data, e.g. via `DM`.
	*
	*  \param g Connection's handle.
	*  \param array_name Null-terminated string containing the name of the array to download. Must match the array name used in `DM`.
	*  \param first The first element of the array for sub-array downloads. `G_BOUNDS` to omit.
	*  \param last The last element of the array for sub-array downloads. `G_BOUNDS` to omit.
	*  \param buffer Buffer containing the null-terminated data to be sent to the controller. The array data may be separated with *carriage return*, *carriage return + line feed*, or a *comma*. No spaces.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  See x_arrays.cpp for an example.
	*/


	//! Uploads array data from the controller's array table.
	GCLIB_DLL_EXPORTED GReturn GCALL GArrayUpload(GCon g, const GCStringIn array_name, GOption first, GOption last, GOption delim, GBufOut buffer, GSize buffer_len);
	/*!<
	*  \param g Connection's handle.
	*  \param array_name Null-terminated string containing the name of the array to upload.
	*  \param first The first element of the array for sub-array uploads. `G_BOUNDS` to omit.
	*  \param last The last element of the array for sub-array uploads. `G_BOUNDS` to omit.
	*  \param delim Sets the delimeter between array elements in the returned data, `G_CR` specifies carriage return, `G_COMMA` specifies comma.
	*  \param buffer Buffer to receive the uploaded data. The data will be null terminated unless function returns `G_BAD_LOST_DATA` due to the buffer being too small to hold the data.
	*  \param buffer_len The length of the receive buffer.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  See x_arrays.cpp for an example.
	*/


	//! Provides a fresh copy of the controller's data record. Data is cast into a union, GDataRecord.
	GCLIB_DLL_EXPORTED GReturn GCALL GRecord(GCon g, union GDataRecord* record, GOption method);
	/*!<
	*  \param g Connection's handle.
	*  \param record A pointer to the user's DataRecord union to hold the copy.
	*  \param method Determines the method for acquiring the data.
	*         * `G_QR`: QR is used via command-and-response.
	*         * `G_DR`: DR is used for asynchronous acquisition.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  When using `G_DR`, the asynchronous data record must already be set up.
	*    * `-s DR` must be used in the GOpen() `address` string to subscribe to records. The driver will automatically set the second argument of `DR`, where applicable.
	*    * `GRecordRate()` should be issued to set `DR` to an appropriate interval, n. The interval must be no faster than the rate at which  GRecord() is called.
	*
	*  GRecord() will block until the data record is received, or the transaction times out.
	*
	*  \note If this function is called with a timeout of zero and the G_DR method, a non-blocking read is performed. 
	*  If a data record has been processed since the last time the function was called,
	*  this data will be returned. If there is not a processed data reecord, but there is data waiting in the socket or PCI FIFO, one read will be performed to process the waiting
	*  data. If new data is still not found after these two attempts, G_GCLIB_NON_BLOCKING_READ_EMPTY will be returned.
	*
	*  See x_grecord.cpp for an example.
	*  See x_nonblocking.cpp for an example of non-blocking usage.
	*/


	//! Provides access to unsolicited messages from the controller.
	GCLIB_DLL_EXPORTED GReturn GCALL GMessage(GCon g, GCStringOut buffer, GSize buffer_len);
	/*!<
	*  To use this function, `-s MG` must be used in the GOpen() `address` string to subscribe to messages.
	*  Unsolicited bytes must be flagged by the high-bit setting, `CW 1`. The driver will automatically set this when subscribing to messages. The user should not overwrite this setting.
	*
	*  Unsolicited messages are data generated by the controller that are not in response to a command, a data record, or an interrupt. Examples follow.
	*  -# Data generated by the `MG` command from embedded code. `MG` sent from the host is solicited.
	*  -# Any command in an embedded program that returns data, e.g. `TP`, `RP`, `var=?`
	*  -# A run time error in an embedded program, e.g. `?55 i=var`
	*
	*  \note Messages are unframed byte streams. There is no guarantee that the user will get complete messages or single messages in a call to GMessage().
	*
	*  \param g Connection's handle.
	*  \param buffer The buffer to write the message data. The buffer will be null terminated.
	*  \param buffer_len The length of the user's buffer.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  GMessage() will block until a message is received, or the function times out. 
	*
	*  \note If this function is called with a timeout of zero, a non-blocking read is performed. If message data has been processed since the last time the function was called,
	*  this data will be returned. If there is no processed message data, but there is data waiting in the socket or PCI FIFO, one read will be performed to process the waiting
	*  data. If new data is still not found after these two attempts, G_GCLIB_NON_BLOCKING_READ_EMPTY will be returned.
	*
	*  \warning When sending message streams through gcaps, the following non-printable bytes are illegal, `$00-$07` and `$10-$17`. These bytes may be routed to a third party 
	*  device such as am HMI or display panel. See MG and CF.
	*
	*  See x_gmessage.cpp for an example.
	*  See x_nonblocking.cpp for an example of non-blocking usage.
	*/


	//! Provides access to PCI and UDP interrupts from the controller.
	GCLIB_DLL_EXPORTED GReturn GCALL GInterrupt(GCon g, GStatus* status_byte);
	/*!<
	*  Interrupts can be generated automatically by the firmware on important events via `EI` (Enable Interrupt) or by the user in embedded DMC code via `UI` (User Interrupt).
	*  To use this function, `-s EI` must be used in the GOpen() address string to subscribe to interrupts.
	*
	*  \param g  Connection's handle.
	*  \param status_byte A pointer to a GStatus to receive the status byte.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  GInterrupt() will block until an interrupt is received, or the function times out.  
	*
	*  \note If this function is called with a timeout of zero, a non-blocking read is performed. If interrupt data is waiting in the interrupt queue,
	*  the oldest byte will be popped off the queue. If there is no interrupt data queued, but there is data waiting in the socket or PCI FIFO, one read will be performed to process the waiting
	*  data. If new data is still not found after these two attempts, G_GCLIB_NON_BLOCKING_READ_EMPTY will be returned.
	*
	*  See x_ginterrupt.cpp for an example.
	*  See x_nonblocking.cpp for an example of non-blocking usage.
	*/


	//! Upgrade firmware.
	GCLIB_DLL_EXPORTED GReturn GCALL GFirmwareDownload(GCon g, GCStringIn filepath);
	/*!<
	*  \param g  Connection's handle.
	*  \param filepath The full file path to the Galil-supplied firmware hex file. See http://www.galil.com/downloads/firmware
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  \code{.cpp}
	*  ec(GInfo(g, buf, sizeof(buf))); //get conntroller info
	*  cout << buf << '\n'; //print the info
	*  ec(GFirmwareDownload(g, "F:/1806.dmc/dmc-1806-r11a.hex"));
	*  ec(GInfo(g, buf, sizeof(buf))); //get the info again
	*  cout << buf << '\n';
	*  // example output:
	*  // GALILPCI1, DMC1846 Rev 1.1a-CM, 4232
	*  // GALILPCI1, DMC1846 Rev 1.1a, 4232
	*  \endcode
	*/


	//! Provides read/write access to driver settings and convenience features based on the request variable.
	GCLIB_DLL_EXPORTED GReturn GCALL GUtility(GCon g, GOption request, GMemory memory1, GMemory memory2);
	/*!<
	*
	*  \note The open source library, gclibo.h, has wrappers for most of these utilities.
	*
	*  \param g Connection's handle.
	*  \param request Defines the request. Input/Output and type of memory are implicit in the value of request. The following lists the supported request values.
	*    * `G_UTIL_TIMEOUT` Read initial timeout value, as specified in GOpen() via `--timeout` switch. 
	*      * `memory1` is output and must be a pointer to an unsigned short. 
	*      * `memory2` is ignored, use null.
	*
	*    * `G_UTIL_TIMEOUT_OVERRIDE` See GTimeout(). Write/Read override timeout value. 
	*      * `memory1` is input. If nonnull, value must be a pointer to a short which overrides the timeout. Write `G_USE_INITIAL_TIMEOUT` to use initial timeout. If null, no write occurs. 
	*      * `memory2` is output. If nonnul, value must be a pointer to a short which will be filled with the current override. `G_USE_INITIAL_TIMEOUT` indicates initial timeout used. If null, no read occurs. `memory2` is processed before 'memory1`.
	*
	*    * `G_UTIL_VERSION` See GVersion(). Returns the library version. A valid connection (g) is not necessary, e.g. g may be null.
	*      * `memory1` is output, and must be a `GCStringOut`. Data will be null terminated, even if the data must be truncated to do so. 
	*      * `memory2` is input and must be a pointer to a `GSize` holding the length of the buffer in `memory1`.
	*
	*    * `G_UTIL_INFO` See GInfo(). Returns information about the connection.
	*      * `memory1` is output and must be a `GCStringOut`. Data will be null terminated, even if the data must be truncated to do so.
	*      * `memory2` is input and must be a pointer to a `GSize` holding the length of the buffer in `memory1`.
	*
	*    * `G_UTIL_SLEEP` See GSleep(). Platform-independent sleep. A valid connection (g) is not necessary, i.e. g may be null.
	*      * `memory1` is input and must be a pointer to an unsigned int, units are milliseconds. 
	*      * `memory2` is ignored, use null.
	*
	*    * `G_UTIL_ADDRESSES` Provides a \\n delimited listing of all available IP addresses, PCI addresses, and COM ports.
	*        A valid connection (g) is not necessary, i.e. g may be null.
	*      * `memory1` is output and must be a `GCStringOut`. Data will be null terminated, even if the data must be truncated to do so. 
	*      * `memory2` is input and must be a pointer to a `GSize` holding the length of the buffer in `memory1`.
	*
	*    * `G_UTIL_IPREQUEST` Listens and returns a \\n delimited listing of Galil MAC addresses sending BOOT-P or DHCP requests.
	*        The function will listen, and block, for roughly 5 seconds. A valid connection (g) is not necessary, i.e. g may be null.
	*      * `memory1` is output and must be a `GCStringOut`. Data will be null terminated, even if the data must be truncated to do so.
	*      * `memory2` is input and must be a pointer to a `GSize` holding the length of the buffer in `memory1`.
	*
	*    * `G_UTIL_ASSIGN` Provides a method to assign an IP address given a Galil MAC address and optionally a host NIC IP address.
	*        A valid connection (g) is not necessary, i.e. g may be null. The address to assign will be pinged to ensure its availability before the assign packet is sent.
	*        `G_GCLIB_UTILITY_IP_TAKEN` will be returned by GUtility() if the ping returns a response.
	*      * `memory1` is input and must be a pointer to a `GCStringIn` containing the address that is to be assigned. e.g. `"192.168.0.43"`
	*      * `memory2` is input and must be a pointer to a `GCStringIn` containing a comma-separated list with the controller's MAC address, followed
	*         by the IP address of the host's network interface to dispatch the assign packet. e.g. `"00:50:4C:20:01:23, 192.168.0.42"`\n
	*         If the host IP is omitted, the assign packet is sent out on all network interfaces. e.g. `"00:50:4C:20:01:23"`
	*
	*    * `G_UTIL_GCAPS_KEEPALIVE` Provides method for kicking the gcaps server. After a default period of 10 minutes of inactivity, gcaps will disconnect the gclib client. To prevent a disconnect,
	*        communicate with the hardware or call `G_UTIL_GCAPS_KEEPALIVE` within the timeout period to reset the timer. The current interval can be optionally read and overwritten.
	*      * `memory1` is output. If nonnull, value must be a pointer to a `GSIZE` which will be filled with the current gcaps timeout, in ms.
	*      * `memory2` is input. If nonnull, value must be a pointer to a `GSIZE` which overrides the current gcaps timeout, in ms, for this connection.
	*
	*    * `G_UTIL_DEVICE_INITIALIZE` Provides a method to reinitialize a connection after a reset, e.g. an RS command. Depending on the device type, the appropriate commands will be sent to
	*      configure the communication bus for optimal performance.
	*      * `memory1` is ignored, use null.
	*      * `memory2` is ignored, use null.
	*
	*
	*  \param memory1 An untyped pointer to data type required for request.
	*  \param memory2 An untyped pointer to data type required for request.
	*
	*  \return The success status or error code of the function. See gclib_errors.h for possible values.
	*
	*  See source of gclibo.c for examples of `G_UTIL_TIMEOUT`, `G_UTIL_TIMEOUT_OVERRIDE`, `G_UTIL_VERSION`, `G_UTIL_INFO`, `G_UTIL_SLEEP`.
	*
	*  Except for serial ports, each line from `G_UTIL_ADDRESSES` will be of the form *address, revision report (^R^V)*.
	*  \code{.unparsed}
	*  10.1.3.168, DMC30010 Rev 1.2d
	*  GALILPCI1, DMC1826 Rev 1.1a
	*  COM7
	*  COM8
	*  \endcode
	*  See GAddresses() for an example of `G_UTIL_ADDRESSES`.
	*
	*  \note Linux/OS X users must be root to use G_UTIL_IPREQUEST and have UDP access to bind and listen on port 67. 
	*
	*  Each line from `G_UTIL_IPREQUEST` will be of the form *model, serial_number, mac*.
    *  \code{.unparsed}
	*  DMC4000, 291, 00:50:4c:20:01:23
	*  DMC30000, 4184, 00:50:4c:40:10:58
	*  \endcode
	*
	*  \code{.cpp}
	*  //example for getting controllers requesting IPs
	*  char buf[1024];
	*  GSize len = sizeof(buf);
	*  GUtility(0, G_UTIL_IPREQUEST, buf, &len);
	*  cout << buf << "\n";
	*  \endcode
	*
	*  \note Linux users must be root to use G_UTIL_ASSIGN and have UDP access to send on port 68.
	*
	*  \code{.cpp}
	*  //example of assigning an IP address.
	*  GUtility(0, G_UTIL_ASSIGN, "10.1.3.178", "00:50:4c:40:10:58"); //Assign 10.1.3.178 TO 00:50:4c:40:10:58
	*  \endcode
	
	*/

#ifdef __cplusplus
} //extern "C"
#endif

#endif //I_D48432D9_1FA3_4C7D_B44C_05F8B9000ADF
