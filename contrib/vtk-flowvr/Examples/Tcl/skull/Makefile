###flowVR BEGIN

APP=skull

DESC_PATH=../../../xml

MML=net/skull.mml.xml

PERL=render-swaplock.pl | render-greedy.pl

ACTION= <application modules run clean graph show_graph>

###flowVR END

DESC := $(shell find $(subst :, ,$(DESC_PATH)) -name "*.desc.xml")
export FLOWVR_DATA_PATH:=$(FLOWVR_DATA_PATH):$(PWD)/../../../data

ifeq ($(MAKECMDGOALS),run)
ifeq ($(VTK_FLOWVR_DATA_ROOT),)
$(warning This example depends on vtk-flowvr-data package. Grab it from flowvr.sf.net.)
$(error VTK_FLOWVR_DATA_ROOT must point to the directory containing the unpacked files.)
endif
endif


.PHONY: all application graph show_graph run clean  modules $(subst |, ,$(PERL))

all: application graph

application: ./tmp/$(APP).run.xml ./tmp/$(APP).net.xml ./tmp/$(APP).cmd.xml

clean:
	@rm -rfv ./tmp/$(APP).*

./tmp/$(APP).run.xml : $(MML) $(DESC)
	@mkdir -p $(@D)
	@echo
	@echo '*** Generating run commands in file $@ ***'
	flowvr-module -d $(DESC_PATH) -mml $< -run -ro $@

./tmp/$(APP).mod.xml : $(MML) $(DESC)
	@mkdir -p $(@D)
	@echo
	@echo "*** Generating modules in file $@ ***"
	flowvr-module -d $(DESC_PATH) -mml $< -mod -mo $@

./tmp/$(APP).net.xml : ./tmp/$(APP).mod.xml $(subst |, ,$(PERL))
	@mkdir -p $(@D)
	@echo
	@echo "*** Generating network in file $@ ***"
	cat $< | $(PERL) > $@

./tmp/$(APP).cmd.xml : ./tmp/$(APP).net.xml
	@mkdir -p $(@D)
	@echo
	@echo "*** Generating commands in file $@ ***"
	flowvr-network -net $< -o $@

run: application
	flowvr-telnet -s -l ./tmp/$(APP).run.xml ./tmp/$(APP).cmd.xml
				
graph: $(APP).ps

%.ps : ./tmp/%.net.xml
	@echo
	@echo "*** Creating network graph in $@ ***"
	@rm -f $@
	flowvr-graph -Tps -o $@ < $<
	@echo
	@echo "*** To open the image: gv $@ (or ggv/kghostview) ***"

show_graph: graph
	gv $(APP).ps &
							
