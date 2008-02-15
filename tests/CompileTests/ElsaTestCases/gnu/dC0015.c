void i2o_lan_handle_event()
{
  int max_evt_data_size = 3;
  struct i2o_reply {
    int data[max_evt_data_size];
  } *evt;
}
