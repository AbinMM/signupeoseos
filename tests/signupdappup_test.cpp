#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include "Runtime/Runtime.h"
#include "eosio.contracts/tests/eosio.system_tester.hpp"

#include <fc/variant_object.hpp>
#include "signupdappup_test.hpp"

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

class signupeoseos_tester : public eosio_system::eosio_system_tester {
	public:

		signupeoseos_tester() {
			produce_blocks(2);

			const asset large_asset = core_from_string("80.0000");
			create_account_with_resources( N(signupeoseos), config::system_account_name, core_from_string("10.0000"), false, large_asset, large_asset );
			//register proxy by first action for this user ever
			BOOST_REQUIRE_EQUAL( success(), push_action(N(signupeoseos), N(regproxy), mvo()
						("proxy",  "signupeoseos")
						("isproxy", true )
						)
					);


			produce_blocks();

			set_code( N(signupeoseos), signupeoseos_wasm() );
			set_abi( N(signupeoseos), signupeoseos_abi().data() );

			produce_blocks();

			set_authority(
					N(signupeoseos),
					"active",
					authority(
						1,
						vector<key_weight>{
							{get_private_key("eosio", "active").get_public_key(), 1},
							{get_private_key("signupeoseos", "active").get_public_key(), 1}
						},
						vector<permission_level_weight>{{{N(signupeoseos), config::eosio_code_name}, 1}}
						),
					"owner",
					{ { N(signupeoseos), "active" } }, { get_private_key( N(signupeoseos), "active" ) }
				     );

			const auto& accnt = control->db().get<account_object,by_name>( N(signupeoseos) );
			abi_def abi;
			BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
			abi_ser.set_abi(abi, abi_serializer_max_time);
		}

		void transfer( name from, name to, const asset& amount, string memo, name manager = config::system_account_name ) {
			base_tester::push_action( N(eosio.token), N(transfer), manager, mutable_variant_object()
					("from",    from)
					("to",      to )
					("quantity", amount)
					("memo", memo)
					);
		}

		abi_serializer abi_ser;
};

BOOST_AUTO_TEST_SUITE(eosio_token_tests)


	BOOST_FIXTURE_TEST_CASE( transfer_tests, signupeoseos_tester ) try {
		auto balance_asset = get_balance(N(tester111111));

		BOOST_REQUIRE_EQUAL(balance_asset, asset(0, symbol(CORE_SYMBOL)));

		// Should have no proxied voting
		BOOST_REQUIRE_EQUAL(stake2votes(core_from_string("0.0000")), get_voter_info(N(signupeoseos))["proxied_vote_weight"].as_double());

		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester111113-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		produce_blocks();

		// Should have voting roght
		BOOST_REQUIRE_EQUAL(stake2votes(core_from_string("0.2000")), get_voter_info(N(signupeoseos))["proxied_vote_weight"].as_double());

		// multiple registry in 1 block
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester111123-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester111133-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester111143-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester111153-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );

		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester222222-tester111153-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester222223-tester222222-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester222224-tester222223-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester222225-tester222224-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );

		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester222231-tester222224-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester222232-tester222224-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester222233-tester222224-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		produce_blocks();

		// Should not be its own referrer

		BOOST_REQUIRE_EXCEPTION(
		   transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester555555-tester555555-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" ),
		   eosio_assert_message_exception,
		   eosio_assert_message_is("Referrer not valid")
		   );
		;

		// Registry should succeed
		auto register_balance = get_balance(N(signupeoseos));
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester111111-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		balance_asset = get_balance(N(tester111111));
		BOOST_REQUIRE_EQUAL(true, balance_asset > asset(0, symbol(CORE_SYMBOL)));
		BOOST_REQUIRE_EQUAL(core_from_string("0.2000"), get_balance(N(signupeoseos)) - register_balance );
		register_balance = get_balance(N(signupeoseos));

		// 1st level referrer will get 0.5 EOS
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester211111-tester111111-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		auto new_asset = get_balance(N(tester111111));
		BOOST_REQUIRE_EQUAL(asset(500, symbol(CORE_SYMBOL)), new_asset-balance_asset);
		balance_asset = new_asset;
		produce_blocks();

		// Refereral should have less than 0.1 EOS as referral fee
		BOOST_REQUIRE_EQUAL(core_from_string("0.0500"), get_balance(N(signupeoseos)) - register_balance);
		register_balance = get_balance(N(signupeoseos));

		// 2st level referrer will get 0.25 EOS
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester311111-tester211111-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		new_asset = get_balance(N(tester111111));
		BOOST_REQUIRE_EQUAL(asset(250, symbol(CORE_SYMBOL)), new_asset-balance_asset);
		BOOST_REQUIRE_EQUAL(core_from_string("0.0250"), get_balance(N(signupeoseos)) - register_balance);
		balance_asset = new_asset;
		register_balance = get_balance(N(signupeoseos));

		produce_blocks();

		// 3rd level referrer will get 0.125 EOS
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester411111-tester311111-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		new_asset = get_balance(N(tester111111));
		BOOST_REQUIRE_EQUAL(asset(125, symbol(CORE_SYMBOL)), new_asset-balance_asset);
		BOOST_REQUIRE_EQUAL(core_from_string("0.0125"), get_balance(N(signupeoseos)) - register_balance);
		balance_asset = new_asset;
		register_balance = get_balance(N(signupeoseos));

		produce_blocks();

		// Only 3 levels of referrals
		transfer( "eosio", "signupeoseos", core_from_string("10.0000"), "tester511111-tester411111-EOS84BLRbGbFahNJEpnnJHYCoW9QPbQEk2iHsHGGS6qcVUq9HhutG" );
		new_asset = get_balance(N(tester111111));
		BOOST_REQUIRE_EQUAL(new_asset, balance_asset);
		BOOST_REQUIRE_EQUAL(core_from_string("0.0125"), get_balance(N(signupeoseos)) - register_balance);

		produce_blocks();

		// Creator cannot transfer createe
		transfer( "signupeoseos", "eosio", core_from_string("1.0000"), "test" , N(signupeoseos));
		BOOST_REQUIRE_THROW(
			transfer( "tester511111", "eosio", core_from_string("1.0000"), "test" , N(signupeoseos)),
			missing_auth_exception
		);

} FC_LOG_AND_RETHROW()


BOOST_AUTO_TEST_SUITE_END()
