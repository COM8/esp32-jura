#pragma once

#include <smooth/core/Application.h>

//---------------------------------------------------------------------------
namespace esp32jura {
//---------------------------------------------------------------------------
#if defined(__cplusplus)
extern "C" {
#endif
void app_main(void);
#if defined(__cplusplus)
}
#endif
//---------------------------------------------------------------------------
class Esp32Jura : public smooth::core::Application {
   private:
   public:
    Esp32Jura();

    void init() override;
    void tick() override;
};
//---------------------------------------------------------------------------
}  // namespace esp32jura
//---------------------------------------------------------------------------
