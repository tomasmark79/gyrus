#pragma once

#include <Utils/UtilsFactory.hpp>

#include <memory>

namespace gyrus::v1 {
  using namespace gyrus::utils;
  class GyrusLib {

  public:
    GyrusLib(const UtilsFactory::ApplicationContext &context);
    ~GyrusLib();
    GyrusLib(const GyrusLib &other) = delete;
    GyrusLib &operator=(const GyrusLib &other) = delete;
    GyrusLib(GyrusLib &&other) = delete;
    GyrusLib &operator=(GyrusLib &&other) = delete;

    [[nodiscard]] bool isInitialized() const noexcept;
    [[nodiscard]]
    const std::shared_ptr<gyrus::assets::IAssetManager> &getAssetManager() const noexcept;
    void performLibraryTask();

  private:
    bool isInitialized_{false};
    static constexpr const char *libName_ = "GyrusLib v0.0.1";

    std::shared_ptr<gyrus::logging::ILogger> logger_;
    std::shared_ptr<gyrus::assets::IAssetManager> assetManager_;
  };

} // namespace gyrus::v1
