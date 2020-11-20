/** (c)2013, Bernard Pratz, bernard at pratz dot net
 *  under the WTFPL License
 */

#ifndef ANCSDefs_h
#define ANCSDefs_h

////////////////////////////////////////////////////////////////////////////////////
#define ANCS_CATEGORY_OTHER             0x0
#define ANCS_CATEGORY_INCOMING_CALL     0x1
#define ANCS_CATEGORY_MISSED_CALL       0x2
#define ANCS_CATEGORY_VOICEMAIL         0x3
#define ANCS_CATEGORY_SOCIAL            0x4
#define ANCS_CATEGORY_SCHEDULE          0x5
#define ANCS_CATEGORY_EMAIL             0x6
#define ANCS_CATEGORY_NEWS              0x7
#define ANCS_CATEGORY_HEALTH_FITNESS    0x8
#define ANCS_CATEGORY_BUSINESS_FINANCE  0x9
#define ANCS_CATEGORY_LOCATION          0xA
#define ANCS_CATEGORY_ENTERTAINMENT     0xB
// reserved ids up to 0xFF

#define ANCS_EVT_NOTIFICATION_ADDED     0x0
#define ANCS_EVT_NOTIFICATION_MODIFIED  0x1
#define ANCS_EVT_NOTIFICATION_REMOVED   0x2
// reserved ids up to 0xFF

#define ANCS_EVT_FLAG_SILENT            0x1
#define ANCS_EVT_FLAG_IMPORTANT         0x2
#define ANCS_EVT_FLAG_PREEXISTING       0x4
#define ANCS_EVT_FLAG_POSITIVEACTION    0x8
#define ANCS_EVT_FLAG_NEGATIVEACTION    0x10
// reserved flags 0x20, 0x40 and 0x80

#define ANCS_COMMAND_GET_NOTIF_ATTRIBUTES         0x0
#define ANCS_COMMAND_GET_APP_ATTRIBUTES           0x1
#define ANCS_COMMAND_PERFORM_NOTIFICATION_ACTION  0x2
// reserved ids up to 0xFF

#define ANCS_NOTIFICATION_ATTRIBUTE_APP_IDENTIFIER         0x0
#define ANCS_NOTIFICATION_ATTRIBUTE_TITLE                  0x1 // shall be followed by 2bytes max length
#define ANCS_NOTIFICATION_ATTRIBUTE_SUBTITLE               0x2 // shall be followed by 2bytes max length
#define ANCS_NOTIFICATION_ATTRIBUTE_MESSAGE                0x3 // shall be followed by 2bytes max length
#define ANCS_NOTIFICATION_ATTRIBUTE_MESSAGE_SIZE           0x4
#define ANCS_NOTIFICATION_ATTRIBUTE_DATE                   0x5
#define ANCS_NOTIFICATION_ATTRIBUTE_POSITIVE_ACTION_LABEL  0x6
#define ANCS_NOTIFICATION_ATTRIBUTE_NEGATIVE_ACTION_LABEL  0x7
// reserved ids up to 0xFF

#define ANCS_ACTION_POSITIVE   0x0
#define ANCS_ACTION_NEGATIVE   0x1
// reserved ids up to 0xFF

#define ANCS_NOTIFICATION_APP_ID_DISPLAY_NAME       0x0
// reserved ids up to 0xFF

#endif

