include $(top_srcdir)/config/Rules.am

VPATH = \
	$(top_srcdir)/module/zfs \
	$(top_srcdir)/module/zcommon \
	$(top_srcdir)/lib/libzpool

AM_CFLAGS += $(DEBUG_STACKFLAGS) $(FRAME_LARGER_THAN)

DEFAULT_INCLUDES += \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/lib/libspl/include

lib_LTLIBRARIES = libzpool.la

<<<<<<< 6bd0df1ce5cce8d1795a74f8a19072ed16db87b2
USER_C = \
	kernel.c \
	taskq.c \
	util.c

KERNEL_C = \
	zfs_comutil.c \
	zfs_deleg.c \
	zfs_fletcher.c \
	zfs_namecheck.c \
	zfs_prop.c \
	zfs_uio.c \
	zpool_prop.c \
	zprop_common.c \
	abd.c \
	arc.c \
	blkptr.c \
	bplist.c \
	bpobj.c \
	bptree.c \
	bqueue.c \
	dbuf.c \
	dbuf_stats.c \
	ddt.c \
	ddt_zap.c \
	dmu.c \
	dmu_diff.c \
	dmu_object.c \
	dmu_objset.c \
	dmu_send.c \
	dmu_traverse.c \
	dmu_tx.c \
	dmu_zfetch.c \
	dnode.c \
	dnode_sync.c \
	dsl_bookmark.c \
	dsl_dataset.c \
	dsl_deadlist.c \
	dsl_deleg.c \
	dsl_dir.c \
	dsl_pool.c \
	dsl_prop.c \
	dsl_scan.c \
	dsl_synctask.c \
	dsl_destroy.c \
	dsl_userhold.c \
	fm.c \
	gzip.c \
	lzjb.c \
	lz4.c \
	metaslab.c \
	multilist.c \
	range_tree.c \
	refcount.c \
	rrwlock.c \
	sa.c \
	sha256.c \
	spa.c \
	spa_boot.c \
	spa_config.c \
	spa_errlog.c \
	spa_history.c \
	spa_misc.c \
	spa_stats.c \
	space_map.c \
	space_reftree.c \
	txg.c \
	trace.c \
	uberblock.c \
	unique.c \
	vdev.c \
	vdev_cache.c \
	vdev_file.c \
	vdev_label.c \
	vdev_mirror.c \
	vdev_missing.c \
	vdev_queue.c \
	vdev_raidz.c \
	vdev_raidz_sse.c \
	vdev_raidz_avx128.c \
	vdev_raidz_avx2.c \
	vdev_raidz_neonv8.c \
	vdev_root.c \
	zap.c \
	zap_leaf.c \
	zap_micro.c \
	zfeature.c \
	zfeature_common.c \
	zfs_byteswap.c \
	zfs_debug.c \
	zfs_fm.c \
	zfs_fuid.c \
	zfs_sa.c \
	zfs_znode.c \
	zil.c \
	zio.c \
	zio_checksum.c \
	zio_compress.c \
	zio_inject.c \
	zle.c \
	zrlock.c

nodist_libzpool_la_SOURCES = \
	$(USER_C) \
	$(KERNEL_C)
=======
libzpool_la_SOURCES = \
	$(top_srcdir)/lib/libzpool/kernel.c \
	$(top_srcdir)/lib/libzpool/taskq.c \
	$(top_srcdir)/lib/libzpool/util.c \
	$(top_srcdir)/module/zcommon/zfs_comutil.c \
	$(top_srcdir)/module/zcommon/zfs_deleg.c \
	$(top_srcdir)/module/zcommon/zfs_fletcher.c \
	$(top_srcdir)/module/zcommon/zfs_namecheck.c \
	$(top_srcdir)/module/zcommon/zfs_prop.c \
	$(top_srcdir)/module/zcommon/zfs_uio.c \
	$(top_srcdir)/module/zcommon/zpool_prop.c \
	$(top_srcdir)/module/zcommon/zprop_common.c \
	$(top_srcdir)/module/zfs/abd.c \
	$(top_srcdir)/module/zfs/arc.c \
	$(top_srcdir)/module/zfs/blkptr.c \
	$(top_srcdir)/module/zfs/bplist.c \
	$(top_srcdir)/module/zfs/bpobj.c \
	$(top_srcdir)/module/zfs/bptree.c \
	$(top_srcdir)/module/zfs/dbuf.c \
	$(top_srcdir)/module/zfs/dbuf_stats.c \
	$(top_srcdir)/module/zfs/ddt.c \
	$(top_srcdir)/module/zfs/ddt_zap.c \
	$(top_srcdir)/module/zfs/dmu.c \
	$(top_srcdir)/module/zfs/dmu_diff.c \
	$(top_srcdir)/module/zfs/dmu_object.c \
	$(top_srcdir)/module/zfs/dmu_objset.c \
	$(top_srcdir)/module/zfs/dmu_send.c \
	$(top_srcdir)/module/zfs/dmu_traverse.c \
	$(top_srcdir)/module/zfs/dmu_tx.c \
	$(top_srcdir)/module/zfs/dmu_zfetch.c \
	$(top_srcdir)/module/zfs/dnode.c \
	$(top_srcdir)/module/zfs/dnode_sync.c \
	$(top_srcdir)/module/zfs/dsl_bookmark.c \
	$(top_srcdir)/module/zfs/dsl_dataset.c \
	$(top_srcdir)/module/zfs/dsl_deadlist.c \
	$(top_srcdir)/module/zfs/dsl_deleg.c \
	$(top_srcdir)/module/zfs/dsl_dir.c \
	$(top_srcdir)/module/zfs/dsl_pool.c \
	$(top_srcdir)/module/zfs/dsl_prop.c \
	$(top_srcdir)/module/zfs/dsl_scan.c \
	$(top_srcdir)/module/zfs/dsl_synctask.c \
	$(top_srcdir)/module/zfs/dsl_destroy.c \
	$(top_srcdir)/module/zfs/dsl_userhold.c \
	$(top_srcdir)/module/zfs/fm.c \
	$(top_srcdir)/module/zfs/gzip.c \
	$(top_srcdir)/module/zfs/lzjb.c \
	$(top_srcdir)/module/zfs/lz4.c \
	$(top_srcdir)/module/zfs/metaslab.c \
	$(top_srcdir)/module/zfs/multilist.c \
	$(top_srcdir)/module/zfs/range_tree.c \
	$(top_srcdir)/module/zfs/refcount.c \
	$(top_srcdir)/module/zfs/rrwlock.c \
	$(top_srcdir)/module/zfs/sa.c \
	$(top_srcdir)/module/zfs/sha256.c \
	$(top_srcdir)/module/zfs/spa.c \
	$(top_srcdir)/module/zfs/spa_boot.c \
	$(top_srcdir)/module/zfs/spa_config.c \
	$(top_srcdir)/module/zfs/spa_errlog.c \
	$(top_srcdir)/module/zfs/spa_history.c \
	$(top_srcdir)/module/zfs/spa_misc.c \
	$(top_srcdir)/module/zfs/spa_stats.c \
	$(top_srcdir)/module/zfs/space_map.c \
	$(top_srcdir)/module/zfs/space_reftree.c \
	$(top_srcdir)/module/zfs/txg.c \
	$(top_srcdir)/module/zfs/trace.c \
	$(top_srcdir)/module/zfs/uberblock.c \
	$(top_srcdir)/module/zfs/unique.c \
	$(top_srcdir)/module/zfs/vdev.c \
	$(top_srcdir)/module/zfs/vdev_cache.c \
	$(top_srcdir)/module/zfs/vdev_file.c \
	$(top_srcdir)/module/zfs/vdev_label.c \
	$(top_srcdir)/module/zfs/vdev_mirror.c \
	$(top_srcdir)/module/zfs/vdev_missing.c \
	$(top_srcdir)/module/zfs/vdev_queue.c \
	$(top_srcdir)/module/zfs/vdev_raidz.c \
	$(top_srcdir)/module/zfs/vdev_raidz_sse.c \
	$(top_srcdir)/module/zfs/vdev_raidz_avx128.c \
	$(top_srcdir)/module/zfs/vdev_raidz_avx2.c \
	$(top_srcdir)/module/zfs/vdev_raidz_neonv8.c \
	$(top_srcdir)/module/zfs/vdev_root.c \
	$(top_srcdir)/module/zfs/zap.c \
	$(top_srcdir)/module/zfs/zap_leaf.c \
	$(top_srcdir)/module/zfs/zap_micro.c \
	$(top_srcdir)/module/zfs/zfeature.c \
	$(top_srcdir)/module/zfs/zfeature_common.c \
	$(top_srcdir)/module/zfs/zfs_byteswap.c \
	$(top_srcdir)/module/zfs/zfs_debug.c \
	$(top_srcdir)/module/zfs/zfs_fm.c \
	$(top_srcdir)/module/zfs/zfs_fuid.c \
	$(top_srcdir)/module/zfs/zfs_sa.c \
	$(top_srcdir)/module/zfs/zfs_znode.c \
	$(top_srcdir)/module/zfs/zil.c \
	$(top_srcdir)/module/zfs/zio.c \
	$(top_srcdir)/module/zfs/zio_checksum.c \
	$(top_srcdir)/module/zfs/zio_compress.c \
	$(top_srcdir)/module/zfs/zio_inject.c \
	$(top_srcdir)/module/zfs/zle.c \
	$(top_srcdir)/module/zfs/zrlock.c
>>>>>>> Select the checksumming implementations by measuring instead of guessing, and add Aarch64 NEON support.

libzpool_la_LIBADD = \
	$(top_builddir)/lib/libunicode/libunicode.la \
	$(top_builddir)/lib/libuutil/libuutil.la \
	$(top_builddir)/lib/libnvpair/libnvpair.la

libzpool_la_LIBADD += $(ZLIB)
libzpool_la_LDFLAGS = -version-info 2:0:0

EXTRA_DIST = $(USER_C)
