###flowVR BEGIN

APP=quadric

DESC_PATH=../../../xml

MML=./net/quadric.mml.xml

PERL=render-swaplock.pl | render-fifo.pl 

ACTION= <application modules run clean graph show_graph>

###flowVR END

DESC := $(shell find $(subst :, ,$(DESC_PATH)) -name "*.desc.xml")

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
							
