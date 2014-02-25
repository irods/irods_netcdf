SUBS = lib rsNcRegGlobalAttr rsNcOpenGroup rsNcOpen rsNcInqWithId rsNcInqId rsNcInqGrps rsNcInq rsNcGetVarsByType rsNcGetAggInfo rsNcGetAggElement rsNcCreate rsNcClose rsNcArchTimeSeries


######################################################################
# Configuration should occur above this line

GCC = g++ 

.PHONY: ${SUBS} clean

default: ${SUBS}

${SUBS}:
	${MAKE} -C $@

clean:
	@-for dir in ${SUBS}; do \
	${MAKE} -C $$dir clean; \
	done
