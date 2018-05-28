#include <eosiolib/eosio.hpp>

using namespace eosio;

class cooltoken : public eosio::contract {
public:
    cooltoken( account_name self )
            :contract(self),_accounts( _self, _self){}

    ///Transfer from one account to another with desired quantity.
    ///@abi action
    void transfer( name from, name to, uint64_t quantity ) {
        //Require the auth from the sender
        require_auth( from );

        //Fetch the from account from the table
        const auto& fromacnt = _accounts.get( from );

        //Assert that the senders balance is sufficient for the transfer
        eosio_assert( fromacnt.balance >= quantity, "overdrawn balance" );

        //Modify the senders balance
        _accounts.modify( fromacnt, from, [&]( auto& a ){ a.balance -= quantity; } );

        //Add the funds to the recipient
        add_balance( from, to, quantity );
    }

    ///Airdrop 100 tokens the the recipient account.
    ///@abi action
    void drop(name to){
        //Require the auth from the receiver
        require_auth(to);

        //Setting a quantity variable that we will use later on.
        uint64_t quantity = 100;

        //Getting the from account from the table
        const auto& fromacc = _accounts.get(_self);

        //Assert that the senders balance is sufficient for the transfer.
        eosio_assert(fromacc.balance >= quantity, "out of tokens");

        //Modify the senders balance
        _accounts.modify(fromacc, _self, [&](auto& a){
            a.balance -= quantity;
        });
        //Add the dropped tokens to the recipient
        add_drop(to, quantity);
    }

    ///Issue new tokens to the specified account.
    ///@abi action
    void issue( name to, uint64_t quantity ) {
        //Require the auth from the contract account
        require_auth( _self );

        //Add the new tokens to the issuer
        add_balance( _self, to, quantity );
    }

private:
    ///Definition of the table struct that holds account balances.
    ///@abi table accounts i64
    struct account {
        name owner;
        uint64_t     balance;

        uint64_t primary_key()const { return owner; }
    };

    //Declaration of Multi-index table to hold accounts
    eosio::multi_index<N(accounts), account> _accounts;


    ///Add the aidrop tokens to the recipient account
    void add_drop(name to, uint64_t quantity){
        //Find the receiver account
        auto toitr = _accounts.find( to );

        //Assert that they haven't already got an account.
        eosio_assert( toitr == _accounts.end(), "Already participated" );

        //Add a new account with the new balance.
        _accounts.emplace( to, [&]( auto& a ) {
            a.owner = to;
            a.balance = quantity;
        });

    }

    ///Add the balance in q to the recipient. Used in transfer action.
    void add_balance( account_name payer, name to, uint64_t q ) {
        //Find the receiver account
        auto toitr = _accounts.find( to );

        //Check if they already have an account or not
        if( toitr == _accounts.end() ) {
            //If they dont' have an account, create a new one and set the correct balance.
            _accounts.emplace( payer, [&]( auto& a ) {
                a.owner = to;
                a.balance = q;
            });
        } else {
            //If they do have an account. Add the new quantity to their balance.
            _accounts.modify( toitr, 0, [&]( auto& a ) {
                a.balance += q;
                eosio_assert( a.balance >= q, "overflow detected" );
            });
        }
    }
};

EOSIO_ABI( cooltoken, (transfer)(issue)(drop) )
