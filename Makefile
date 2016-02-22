VERSION  = 1.0
MAKESELF = ./makeself.sh
TAR_TEMP = build
OSARCH  := $(subst /,-,$(shell uname -m | tr ' ' _))
X86SUFFIX = $(if $(filter $(OSARCH),i686),/x86,)
DYLIBS    = uarc_rtdbms uarc_express
SUBUARC   = uarc_colld uarc_intfd  uarc_rmemd

all: SharedLib UarcProgram 
	@rm -rf build/log/* build/tmp/* uarc_install_*.run
	@echo $(VERSION) > build/etc/uarc/version
	@$(MAKESELF) --bzip2 --current --nomd5 --nocrc $(TAR_TEMP) uarc_install_$(VERSION)_x86_64.run "Energy control platform acquisition subsystem" ./autorun.sh 
	@echo "安装脚本已生成"
	
clean:
	-for d in $(DYLIBS);  do (cd src/$$d/Release; make -s clean ); done
	-for d in $(SUBUARC); do (cd src/$$d/Release; make -s clean ); done

SharedLib:
	@for d in $(DYLIBS);  do (cd src/$$d/Release; make -s PATH_LIB=../../../$(TAR_TEMP)/lib PATH_INCLUDE=../../../include/; ); done
	@for d in $(DYLIBS); do (cp -rf src/$$d/Release/lib$$d.so $(TAR_TEMP)/lib/; ); done
	@for d in $(DYLIBS); do (cp -rf src/$$d/src/*.h $(TAR_TEMP)/../include/; ); done
	@echo "动态库编译完毕！"

UarcProgram:
	@for d in $(SUBUARC); do (cd src/$$d/Release; make -s PATH_LIB=../../../$(TAR_TEMP)/lib PATH_INCLUDE=../../../include/; ); done
	@for d in $(SUBUARC); do (cp -rf src/$$d/Release/$$d $(TAR_TEMP)/bin/); done
	@echo "全部工程编译完成！"

CreateScript:
	@echo "#!/bin/bash" > $(PWD)/build/bin/uarc_start.sh
	@echo "echo -n 启动redis-server...;/usr/bin/redis-server /etc/uarc/redis.conf;sleep 5;echo 完成！;sleep 2;" >> $(PWD)/build/bin/uarc_start.sh
	@for d in $(SUBUARC); do (echo "echo -n 启动"$$d"...;/home/uarc/bin/"$$d" --daemon;sleep 2;echo 完成！;sleep 2;" >> $(PWD)/build/bin/uarc_start.sh); done
	@echo "echo 程序启动完毕！" >> $(PWD)/build/bin/uarc_start.sh
	@chmod +x $(PWD)/build/bin/uarc_start.sh
	@echo "#!/bin/bash" > $(PWD)/build/bin/uarc_stop.sh
	@for d in $(SUBUARC); do (echo "killall "$$d >> $(PWD)/build/bin/uarc_stop.sh); done
	@echo "killall redis-server" >> $(PWD)/build/bin/uarc_stop.sh
	@echo "echo 程序已停止！" >> $(PWD)/build/bin/uarc_stop.sh
	@chmod +x $(PWD)/build/bin/uarc_stop.sh
	
