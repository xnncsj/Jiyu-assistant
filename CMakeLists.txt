cmake_minimum_required(VERSION 3.10)
project(jiyu VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找wxWidgets包，添加更多组件
find_package(wxWidgets REQUIRED COMPONENTS core base adv aui)
include(${wxWidgets_USE_FILE})

# 添加可执行文件
add_executable(jiyu WIN32 main.cpp)

# 链接wxWidgets库
target_link_libraries(jiyu ${wxWidgets_LIBRARIES})

# 如果是MSVC编译器，添加UTF-8编码支持和图标
if(MSVC)
    target_compile_options(jiyu PRIVATE "/utf-8")
    # 添加管理员权限的manifest文件和DPI感知支持
    set_target_properties(jiyu PROPERTIES 
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /MANIFESTINPUT:dpiaware.manifest")
    
    # 创建DPI感知manifest文件
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dpiaware.manifest 
"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>
<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\" xmlns:asmv3=\"urn:schemas-microsoft-com:asm.v3\">
  <asmv3:application>
    <asmv3:windowsSettings>
      <dpiAware xmlns=\"http://schemas.microsoft.com/SMI/2005/WindowsSettings\">true/pm</dpiAware>
      <dpiAwareness xmlns=\"http://schemas.microsoft.com/SMI/2016/WindowsSettings\">PerMonitorV2</dpiAwareness>
    </asmv3:windowsSettings>
  </asmv3:application>
</assembly>")
endif()

# 安装配置
install(TARGETS jiyu DESTINATION bin) 