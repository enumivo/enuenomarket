#include <enulib/currency.hpp>
#include <enulib/enu.hpp>
#include <vector>

#define ENU_SYMBOL S(4, ENU)  
#define ENO_SYMBOL S(4, ENO)  

using namespace enumivo;

class ex : public contract {
 public:
  ex(account_name self)
      : contract(self) {}

  void receivedenu(const currency::transfer& transfer);
  void receivedeno(const currency::transfer& transfer);

  void apply(account_name contract, action_name act);

};
