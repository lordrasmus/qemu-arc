arc_ss = ss.source_set()
arc_ss.add(files('boot.c'))
arc_ss.add(when: 'CONFIG_ARC_SANDBOX', if_true: files('sandbox.c'))

hw_arch += {'arc': arc_ss}
