#include <eosiolib/eosio.hpp>

class token : public eosio::contract {
   public:
      token( account_name self )
      :contract(self),_accounts( _self, _self){}

      [[eosio::action]]
      void transfer( account_name from, account_name to, uint64_t quantity ) {
         require_auth( from );

         const auto& fromacnt = _accounts.get( from );
         eosio_assert( fromacnt.balance >= quantity, "overdrawn balance" );
         _accounts.modify( fromacnt, from, [&]( auto& a ){ a.balance -= quantity; } );

         add_balance( from, to, quantity );
      }

      [[eosio::action]]
      void issue( account_name to, uint64_t quantity ) {
         require_auth( _self );
         add_balance( _self, to, quantity );
      }

      [[eosio::action]]
      void drop(account_name to){
        require_auth(to);
        uint64_t quantity = 100;

        const auto& fromacc = _accounts.get(_self);

        eosio_assert(fromacc.balance >= quantity, "out of tokens");

        _accounts.modify(fromacc, _self, [&](auto& row){
          row.balance -= quantity;
        });

        add_drop(to, quantity);
      }

   private:
      struct [[eosio::table]] account {
         account_name owner;
         uint64_t     balance;

         uint64_t primary_key()const { return owner; }
      };

      eosio::multi_index<N(accounts), account> _accounts;

      void add_balance( account_name payer, account_name to, uint64_t q ) {
         auto toitr = _accounts.find( to );
         if( toitr == _accounts.end() ) {
           _accounts.emplace( payer, [&]( auto& a ) {
              a.owner = to;
              a.balance = q;
           });
         } else {
           _accounts.modify( toitr, 0, [&]( auto& a ) {
              a.balance += q;
              eosio_assert( a.balance >= q, "overflow detected" );
           });
         }
      }

      void add_drop( account_name to, uint64_t q ) {
         auto toitr = _accounts.find( to );
         eosio_assert(toitr == _accounts.end(), "already participated");
         _accounts.emplace( to, [&]( auto& a ) {
            a.owner = to;
            a.balance = q;
         });
      }
};

EOSIO_ABI( token, (transfer)(issue)(drop) )
