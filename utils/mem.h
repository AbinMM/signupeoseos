#include "exchange/exchange_state.cpp"
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/multi_index.hpp>

using namespace eosio;

class rammarketinfo
{
    public:
        rammarketinfo(account_name sys_account);
        asset get_buying_eos_price_without_fee(uint32_t bytes);
        asset get_buying_eos_price_with_fee(uint32_t bytes);
        uint64_t get_buying_bytes(asset quant);

    protected:
        rammarket _rammarket;
};
