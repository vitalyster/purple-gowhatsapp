#include "gowhatsapp.h"
#include "constants.h"

/*
 * Interprets a message received from whatsmeow. Handles login success and failure. Forwards errors.
 */
void
gowhatsapp_process_message(PurpleAccount *account, gowhatsapp_message_t *gwamsg)
{
    purple_debug_info(
        "gowhatsapp", "recieved message type %d for user %s at %ld id %s remote %s sender %s (fromMe %d, system %d): %s %s\n",
        (int)gwamsg->msgtype,
        gwamsg->username,
        gwamsg->timestamp,
        gwamsg->id,
        gwamsg->remoteJid,
        gwamsg->senderJid,
        gwamsg->fromMe,
        gwamsg->system,
        gwamsg->text,
        gwamsg->quote
    );

    PurpleConnection *pc = purple_account_get_connection(account);

    if (!gwamsg->timestamp) {
        gwamsg->timestamp = time(NULL);
    }
    switch(gwamsg->msgtype) {
        case gowhatsapp_message_type_error:
            purple_connection_error(pc, PURPLE_CONNECTION_ERROR_OTHER_ERROR, gwamsg->text);
            break;
        case gowhatsapp_message_type_login:
            gowhatsapp_display_qrcode(pc, gwamsg->text, gwamsg->blob, gwamsg->blobsize);
            break;
        case gowhatsapp_message_type_connected:
            purple_connection_set_state(pc, PURPLE_CONNECTION_CONNECTED);
            if (purple_account_get_bool(account, GOWHATSAPP_FAKE_ONLINE_OPTION, TRUE)) {
                gowhatsapp_assume_all_buddies_online(account);
            }
            break;
        case gowhatsapp_message_type_text:
            gowhatsapp_display_text_message(pc, gwamsg);
            break;
        case gowhatsapp_message_type_typing:
            serv_got_typing(pc, gwamsg->remoteJid, 0, PURPLE_TYPING);
            break;
        case gowhatsapp_message_type_typing_stopped:
            serv_got_typing_stopped(pc, gwamsg->remoteJid);
            break;
        default:
            purple_debug_info("gowhatsapp", "handling this message type is not implemented");
    }
}
