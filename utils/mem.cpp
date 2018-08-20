#include "mem.h"

rammarketinfo::rammarketinfo(account_name sys_account) : _rammarket(sys_account, sys_account){
    auto itr = _rammarket.find(S(4,RAMCORE));
    eosio_assert( itr != _rammarket.end(), "Rammarket not found" );
};

asset rammarketinfo::get_buying_eos_price_without_fee(uint32_t bytes){
    auto itr = _rammarket.find(S(4,RAMCORE));
    auto tmp = *itr;
    return tmp.convert( asset(bytes, S(0,RAM)), CORE_SYMBOL);
}

asset rammarketinfo::get_buying_eos_price_with_fee(uint32_t bytes){
    auto without_fee = get_buying_eos_price_without_fee(bytes);
    without_fee.amount = (without_fee.amount * 200 + 199) / 199;
    return without_fee;
}

uint64_t rammarketinfo::get_buying_bytes(asset quant){
    auto itr = _rammarket.find(S(4,RAMCORE));
    auto tmp = *itr;
    quant.amount = quant.amount - (quant.amount + 199) / 200;
    return tmp.convert(quant, S(0,RAM)).amount;
}
