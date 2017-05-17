void sendMessage();
void sendMessage_forward() {
// EDG BUG: For C langauge mode this will be "return;" (missing the call to "sendMessage()")
   return sendMessage();
}
