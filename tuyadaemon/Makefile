include $(TOPDIR)/rules.mk

PKG_NAME:=tuya_cloud_daemon
PKG_RELEASE:=1
PKG_VERSION:=2.0.1

include $(INCLUDE_DIR)/package.mk

define Package/tuya_cloud_daemon
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Tuya_Cloud_Daemon
	DEPENDS:=+libtuya +libubus +libubox +libblobmsg-json +USE_UCLIBC:argp-standalone
endef

define Package/tuya_cloud_daemon/description
	This daemon will allow the router to communicate with Tuya cloud interface
endef

define Package/tuya_cloud_daemon/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tuya_daemon $(1)/usr/bin
	$(INSTALL_BIN) ./files/tuya_daemon.init $(1)/etc/init.d/tuya_daemon
	$(INSTALL_DATA) ./files/tuya_daemon.config $(1)/etc/config/tuya_daemon
endef

$(eval $(call BuildPackage,tuya_cloud_daemon))