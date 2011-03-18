#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0xe06cb061, "struct_module" },
	{ 0x4af89d88, "usb_register_dev" },
	{ 0x45a32afa, "usb_get_dev" },
	{ 0x2940bb15, "__mutex_init" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0x83800bfa, "kref_init" },
	{ 0xcefd0586, "kmem_cache_alloc" },
	{ 0x921e3c52, "kmalloc_caches" },
	{ 0x9775cdc, "kref_get" },
	{ 0xf02f82e9, "usb_find_interface" },
	{ 0x1ab74836, "usb_register_driver" },
	{ 0x1ea2576b, "usb_put_dev" },
	{ 0x90d67db7, "usb_control_msg" },
	{ 0x6c2e3320, "strncmp" },
	{ 0x60a4461c, "__up_wakeup" },
	{ 0x37a0cba, "kfree" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x625acc81, "__down_failed_interruptible" },
	{ 0xda4008e6, "cond_resched" },
	{ 0x1b7d4074, "printk" },
	{ 0x87cb3741, "usb_deregister_dev" },
	{ 0xd5b037e1, "kref_put" },
	{ 0x6ce66043, "mutex_unlock" },
	{ 0x78d0ffc4, "mutex_lock" },
	{ 0x35697b06, "usb_deregister" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v16C0p05DCd*dc*dsc*dp*ic*isc*ip*");
