bool
Implementation::query(std::string query, va_list args) const
{
    bool result = false;

    if (query == "nodeIsReachable(int block_id, int context, bool *reachable)")
    {
        result = true;

        DFI_STORE store = (DFI_STORE) analysisInfo;
        int block_id = va_arg(args, int);
        int context = va_arg(args, int);
        CarrierType info = dfi_get_pre(store, block_id, context);

        bool *reachable = va_arg(args, bool *);
        *reachable = (o_itvl_State_isbottom(info) == FLO_FALSE ? true : false);
    }

    return result;
}
