#include "ex.hpp"

#include <cmath>
#include <enulib/action.hpp>
#include <enulib/asset.hpp>
#include "enu.token.hpp"

using namespace enumivo;
using namespace std;

void ex::receivedenu(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.eno.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  
  enu_balance = enu_balance/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  // get ENO balance
  double eno_balance = enumivo::token(N(token.eurno)).
	   get_balance(N(enu.eno.mm), enumivo::symbol_type(ENO_SYMBOL).name()).amount;

  eno_balance = eno_balance/10000;

  //deduct fee
  received = received * 0.997;
  
  double product = eno_balance * enu_balance;

  double buy = eno_balance - (product / (received + enu_balance));

  auto to = transfer.from;

  auto quantity = asset(10000*buy, ENO_SYMBOL);

  action(permission_level{N(enu.eno.mm), N(active)}, N(token.eurno), N(transfer),
         std::make_tuple(N(enu.eno.mm), to, quantity,
                         std::string("Buy ENO with ENU")))
      .send();

  action(permission_level{_self, N(active)}, N(enu.token), N(transfer),
         std::make_tuple(_self, N(enu.eno.mm), transfer.quantity,
                         std::string("Buy ENO with ENU")))
      .send();
}

void ex::receivedeno(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENO balance
  double eno_balance = enumivo::token(N(token.eurno)).
	   get_balance(N(enu.eno.mm), enumivo::symbol_type(ENO_SYMBOL).name()).amount;
  
  eno_balance = eno_balance/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.eno.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;

  enu_balance = enu_balance/10000;

  //deduct fee
  received = received * 0.997;

  double product = enu_balance * eno_balance;

  double sell = enu_balance - (product / (received + eno_balance));

  auto to = transfer.from;

  auto quantity = asset(10000*sell, ENU_SYMBOL);

  action(permission_level{N(enu.eno.mm), N(active)}, N(enu.token), N(transfer),
         std::make_tuple(N(enu.eno.mm), to, quantity,
                         std::string("Sell ENO for ENU")))
      .send();

  action(permission_level{_self, N(active)}, N(token.eurno), N(transfer),
         std::make_tuple(_self, N(enu.eno.mm), transfer.quantity,
                         std::string("Sell ENO for ENU")))
      .send();
      
}

void ex::apply(account_name contract, action_name act) {

  if (contract == N(enu.token) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENU_SYMBOL,
                 "Must send ENU");
    receivedenu(transfer);
    return;
  }

  if (contract == N(token.eurno) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENO_SYMBOL,
                 "Must send ENO");
    receivedeno(transfer);
    return;
  }

  if (act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();
    enumivo_assert(false, "Must send ENO or ENU");
    return;
  }

  if (contract != _self) return;

}

extern "C" {
[[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  ex enueno(receiver);
  enueno.apply(code, action);
  enumivo_exit(0);
}
}
