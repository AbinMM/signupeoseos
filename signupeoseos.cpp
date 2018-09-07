//
// Created by Hongbo Tang on 2018/7/5.
//

#include "signupeoseos.hpp"
#include "utils/mem.cpp"

signupeoseos::st_feeconfig signupeoseos::get_config(){
    return sgt_feeconfig.get_or_create(_self, st_feeconfig{});
}

size_t find_next_part(string input, string * output, bool require_find=true){
    auto separator_pos = input.find(' ');
    if (separator_pos == string::npos) {
        separator_pos = input.find('-');
    }
    if(separator_pos != string::npos){
        *output = input.substr(0, separator_pos);
    }else{
        eosio_assert(!require_find, "Account name and other command must be separated with space or minuses");
    }
    return separator_pos;
}

void signupeoseos::config(uint64_t level, asset fee){
    require_auth(_self);

    eosio_assert(fee.symbol == CORE_SYMBOL, "Fee should be EOS");
    eosio_assert(fee.is_valid(), "Invalid token transfer");
    eosio_assert(fee.amount > 0, "Quantity must be positive");
    eosio_assert(level < 10, "Level should be less than 10");

    auto config = get_config();
    config.maxlevel = level;
    config.fee = fee;

    sgt_feeconfig.set(config, _self);
}

void signupeoseos::distribute_fee(asset total_fee, account_name paying_account, uint64_t level){
    if(level <= 0){
        return;
    }

    auto itr_referrer = referrers.find(paying_account);
    if(itr_referrer != referrers.end()){
        auto referrer = *itr_referrer;
        account_name refer_account = referrer.referrer;
        asset referer_fee = total_fee;
        referer_fee.amount = referer_fee.amount * 1.0 / 2.0;
        action(
            permission_level{ _self, N(active) },
            N(eosio.token),
            N(transfer),
            make_tuple(_self, refer_account, referer_fee, string("Sending referer fee"))
            ).send();

        itr_referrer = referrers.find(refer_account);
        if(itr_referrer != referrers.end()){
        distribute_fee(referer_fee, refer_account, level-1);
        }
    }
}

void signupeoseos::delegate(account_name new_account_name, string public_key_str){
    require_auth(_self);

    action(
        permission_level{ new_account_name, N(active) },
        N(eosio),
        N(voteproducer),
        make_tuple(new_account_name, _self, vector<account_name>{})
    ).send();

    eosio_assert(public_key_str.length() == 53, "Length of publik key should be 53");

    string pubkey_prefix("EOS");
    auto result = mismatch(pubkey_prefix.begin(), pubkey_prefix.end(), public_key_str.begin());
    eosio_assert(result.first == pubkey_prefix.end(), "Public key should be prefix with EOS");
    auto base58substr = public_key_str.substr(pubkey_prefix.length());

    vector<unsigned char> vch;
    eosio_assert(decode_base58(base58substr, vch), "Decode pubkey failed");
    eosio_assert(vch.size() == 37, "Invalid public key");

    array<unsigned char,33> pubkey_data;
    copy_n(vch.begin(), 33, pubkey_data.begin());

    signup_public_key pubkey = {
        .type = 0,
        .data = pubkey_data,
    };

    key_weight pubkey_weight = {
        .key = pubkey,
        .weight = 1,
    };

    authority active = authority{
        .threshold = 1,
        .keys = {pubkey_weight},
        .accounts = {},
        .waits = {}
    };

    action(
            permission_level{ new_account_name, N(active) },
            N(eosio),
            N(updateauth),
            make_tuple(new_account_name, N(active), N(owner), active)
    ).send();
}

void signupeoseos::transfer(account_name from, account_name to, asset quantity, string memo) {
    if (from == _self || to != _self) {
        return;
    }

    eosio_assert(quantity.symbol == CORE_SYMBOL, "signupeoseos only accepts CORE for signup eos account");
    eosio_assert(quantity.is_valid(), "Invalid token transfer");
    eosio_assert(quantity.amount > 0, "Quantity must be positive");

    memo.erase(memo.begin(), find_if(memo.begin(), memo.end(), [](int ch) {
        return !isspace(ch);
    }));
    memo.erase(find_if(memo.rbegin(), memo.rend(), [](int ch) {
        return !isspace(ch);
    }).base(), memo.end());

    string account_name_str;
    auto next_part_pos = find_next_part(memo, &account_name_str);
    string referrer_and_pubkey_str = memo.substr(next_part_pos + 1);

    string refer_name_str, public_key_str;
    next_part_pos = find_next_part(referrer_and_pubkey_str, &refer_name_str, false);

    if(next_part_pos == string::npos){
      public_key_str = referrer_and_pubkey_str;
    }else{
      public_key_str = referrer_and_pubkey_str.substr(next_part_pos + 1);
    }

    eosio_assert(account_name_str.length() == 12, "Length of account name should be 12");
    account_name new_account_name = string_to_name(account_name_str.c_str());
    eosio_assert(!is_account(new_account_name), ("Account already exists: " + account_name_str).c_str());

    account_name referer_account_name = 0;
    if(refer_name_str.length()){
      eosio_assert(refer_name_str.length() == 12, "Length of refer account name should be 12");
      referer_account_name = string_to_name(refer_name_str.c_str());

      eosio_assert(is_account(referer_account_name), "Referrer not valid");
      eosio_assert(referrers.find(new_account_name) == referrers.end(), "Should not reach here");
      referrers.emplace(_self, [&](auto& st_referer) {
          st_referer.user = new_account_name;
          st_referer.referrer = referer_account_name;
          });
    }

    eosio_assert(public_key_str.length() == 53, "Length of publik key should be 53");
    string pubkey_prefix("EOS");
    auto result = mismatch(pubkey_prefix.begin(), pubkey_prefix.end(), public_key_str.begin());
    eosio_assert(result.first == pubkey_prefix.end(), "Public key should be prefix with EOS");
    auto base58substr = public_key_str.substr(pubkey_prefix.length());

    vector<unsigned char> vch;
    eosio_assert(decode_base58(base58substr, vch), "Decode pubkey failed");
    eosio_assert(vch.size() == 37, "Invalid public key");

    array<unsigned char,33> pubkey_data;
    copy_n(vch.begin(), 33, pubkey_data.begin());

    checksum160 check_pubkey;
    ripemd160(reinterpret_cast<char *>(pubkey_data.data()), 33, &check_pubkey);
    eosio_assert(memcmp(&check_pubkey.hash, &vch.end()[-4], 4) == 0, "invalid public key");

    auto _rammarketinfo = rammarketinfo(N(eosio));
    auto buy_ram = _rammarketinfo.get_buying_eos_price_with_fee(1024 * 4); // Need 4k ram to create account

    auto config = get_config();

    asset stake_net(500, CORE_SYMBOL);
    asset stake_cpu(1500, CORE_SYMBOL);
    asset registry_fee = config.fee;

    if(referer_account_name){
        registry_fee /= 2;
    }

    eosio_assert(stake_net + stake_cpu + buy_ram + registry_fee <= quantity, "Not enough eos");
    asset balance = quantity - stake_net - stake_cpu - buy_ram - registry_fee;

    signup_public_key pubkey = {
        .type = 0,
        .data = pubkey_data,
    };
    key_weight pubkey_weight = {
        .key = pubkey,
        .weight = 1,
    };
    authority owner = authority{
        .threshold = 1,
        .keys = {pubkey_weight},
        .accounts = {},
        .waits = {}
    };
    authority active = authority{
        .threshold = 1,
        .keys = {pubkey_weight},
        .accounts = {
            permission_level_weight{
                .permission = permission_level(_self, N(eosio.code)),
                .weight = 1
            },
        },
        .waits = {}
    };

    newaccount new_account = newaccount{
        .creator = _self,
        .name = new_account_name,
        .owner = owner,
        .active = active,
    };

    action(
            permission_level{ _self, N(active) },
            N(eosio),
            N(newaccount),
            new_account
    ).send();

    action(
            permission_level{ _self, N(active)},
            N(eosio),
            N(buyram),
            make_tuple(_self, new_account_name, buy_ram)
    ).send();

    action(
            permission_level{ _self, N(active)},
            N(eosio),
            N(delegatebw),
            make_tuple(_self, new_account_name, stake_net, stake_cpu, true)
    ).send();

    action(
        permission_level{ _self, N(active) },
        N(eosio.token),
        N(transfer),
        make_tuple(_self, new_account_name, balance, string("Sending remaining eos to balance"))
    ).send();

    if(referer_account_name){
      distribute_fee(registry_fee, new_account_name, config.maxlevel);
    }

    eosio::transaction out;
    out.actions.emplace_back(
        permission_level{ _self, N(active) },
        _self,
        N(delegate),
        make_tuple(new_account_name, public_key_str)
        );
    out.send(new_account_name, _self);
}
