# signupeoseos
## Where your moneys go
### Stake and ram

* `0.15` EOS will be staked for CPU  
* `0.05` EOS will be staked for NET  
* Will purchase 4k RAM for account creation
* A minimum of `0.1 EOS` will be charged for registration fee
* The remaining EOS will be transferred to the new account

### Registration fee and Referral Program

If no referrer is provided:

* Need to pay `0.2 EOS` registration fee if there is no referrer;

If referrer is provided:

* Need to pay half of registration fee(`0.1 EOS`)
* The referrer will receive `0.05 EOS` from the registration fee stated above
* If the referrer account is registered through this service and also has a referrer, that offerer will get `0.025 EOS`
* The next level of referrer will get `0.0125 EOS`
* The remaining registration fee goes to this service for dev and running costs

### Voting

Newly created account's voting right will be proxied to this service, in order to help improving EOS's voting rate.

### Usage

```
cleos transfer eosio signupeoseos "5 EOS" "your_account_name-your_referer-your_public_key"
```
