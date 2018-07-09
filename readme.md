# signupeoseos

### Install

```
cleos create account eosio signupeoseo your_public_key
cleos set account permission signupeoseos active '{"threshold": 1,"keys": [{"key": "your_public_key","weight": 1}],"accounts": [{"permission":{"actor":"signupeoseos","permission":"eosio.code"},"weight":1}]}' owner -p signupeoseos
make build_and_deploy
```

### Usage

```
cleos transfer eosio signupeoseos "5 EOS" "your_account_name-your_public_key"
```
