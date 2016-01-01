require 'spec_helper'

RSpec::Matchers.define :have_the_value do |expected|
  match do |actual|
    @failure_message = ""

    vset_for_pmid = actual.vset.find { |v| v.pmid == @pmid }

    unless vset_for_pmid
      @failure_message << "no vset with pmid #{@pmid} found "
      return false
    end

    instance = vset_for_pmid.vlist.find{ |v| v.inst == @instance }
    unless instance
      @failure_message << "no instance of #{@instance} found for pmid #{@pmid} "
      return false
    end

    # Value could be further in a PmValueBlock class if not INSITU
    value = vset_for_pmid.valfmt == PCP::PMAPI::PM_VAL_INSITU ? instance.value : instance.value.vbuf

    unless value == expected
      @failure_message << "value '#{instance.value}' does not match expected value '#{expected}' "
      return false
    end

    # All good if we've got this far
    return true
  end

  chain :for_pmid do |pmid|
    @pmid = pmid
  end

  chain :and_instance do |instance|
    @instance = instance
  end

  failure_message do |actual|
    "#{@failure_message}in #{actual}"
  end
end

describe PCP::PMAPI do

  describe 'constants' do
    [
      PCP::PMAPI::PM_SPACE_BYTE,
      PCP::PMAPI::PM_SPACE_KBYTE,
      PCP::PMAPI::PM_SPACE_MBYTE,
      PCP::PMAPI::PM_SPACE_GBYTE,
      PCP::PMAPI::PM_SPACE_TBYTE,
      PCP::PMAPI::PM_SPACE_PBYTE,
      PCP::PMAPI::PM_SPACE_EBYTE,

      PCP::PMAPI::PM_TIME_NSEC,
      PCP::PMAPI::PM_TIME_USEC,
      PCP::PMAPI::PM_TIME_MSEC,
      PCP::PMAPI::PM_TIME_SEC,
      PCP::PMAPI::PM_TIME_MIN,
      PCP::PMAPI::PM_TIME_HOUR,

      PCP::PMAPI::PM_TYPE_NOSUPPORT,
      PCP::PMAPI::PM_TYPE_32,
      PCP::PMAPI::PM_TYPE_U32,
      PCP::PMAPI::PM_TYPE_64,
      PCP::PMAPI::PM_TYPE_U64,
      PCP::PMAPI::PM_TYPE_FLOAT,
      PCP::PMAPI::PM_TYPE_DOUBLE,
      PCP::PMAPI::PM_TYPE_STRING,
      PCP::PMAPI::PM_TYPE_AGGREGATE,
      PCP::PMAPI::PM_TYPE_AGGREGATE_STATIC,
      PCP::PMAPI::PM_TYPE_EVENT,
      PCP::PMAPI::PM_TYPE_HIGHRES_EVENT,
      PCP::PMAPI::PM_TYPE_UNKNOWN,

      PCP::PMAPI::PM_SEM_COUNTER,
      PCP::PMAPI::PM_SEM_INSTANT,
      PCP::PMAPI::PM_SEM_DISCRETE,


      PCP::PMAPI::PM_CONTEXT_UNDEF,
      PCP::PMAPI::PM_CONTEXT_HOST,
      PCP::PMAPI::PM_CONTEXT_ARCHIVE,
      PCP::PMAPI::PM_CONTEXT_LOCAL,
      PCP::PMAPI::PM_CONTEXT_TYPEMASK,
      PCP::PMAPI::PM_CTXFLAG_SECURE,
      PCP::PMAPI::PM_CTXFLAG_COMPRESS,
      PCP::PMAPI::PM_CTXFLAG_RELAXED,
      PCP::PMAPI::PM_CTXFLAG_AUTH,
      PCP::PMAPI::PM_CTXFLAG_CONTAINER,

      PCP::PMAPI::PMNS_LOCAL,
      PCP::PMAPI::PMNS_REMOTE,
      PCP::PMAPI::PMNS_ARCHIVE,

      PCP::PMAPI::PM_VAL_HDR_SIZE,

      PCP::PMAPI::PM_VAL_INSITU,
      PCP::PMAPI::PM_VAL_DPTR,
      PCP::PMAPI::PM_VAL_SPTR,

      PCP::PMAPI::PM_ERR_GENERIC,
      PCP::PMAPI::PM_ERR_PMNS,
      PCP::PMAPI::PM_ERR_NOPMNS,
      PCP::PMAPI::PM_ERR_DUPPMNS,
      PCP::PMAPI::PM_ERR_TEXT,
      PCP::PMAPI::PM_ERR_APPVERSION,
      PCP::PMAPI::PM_ERR_VALUE,
      PCP::PMAPI::PM_ERR_TIMEOUT,
      PCP::PMAPI::PM_ERR_NODATA,
      PCP::PMAPI::PM_ERR_RESET,
      PCP::PMAPI::PM_ERR_NAME,
      PCP::PMAPI::PM_ERR_PMID,
      PCP::PMAPI::PM_ERR_INDOM,
      PCP::PMAPI::PM_ERR_INST,
      PCP::PMAPI::PM_ERR_UNIT,
      PCP::PMAPI::PM_ERR_CONV,
      PCP::PMAPI::PM_ERR_TRUNC,
      PCP::PMAPI::PM_ERR_SIGN,
      PCP::PMAPI::PM_ERR_PROFILE,
      PCP::PMAPI::PM_ERR_IPC,
      PCP::PMAPI::PM_ERR_EOF,
      PCP::PMAPI::PM_ERR_NOTHOST,
      PCP::PMAPI::PM_ERR_EOL,
      PCP::PMAPI::PM_ERR_MODE,
      PCP::PMAPI::PM_ERR_LABEL,
      PCP::PMAPI::PM_ERR_LOGREC,
      PCP::PMAPI::PM_ERR_NOTARCHIVE,
      PCP::PMAPI::PM_ERR_LOGFILE,
      PCP::PMAPI::PM_ERR_NOCONTEXT,
      PCP::PMAPI::PM_ERR_PROFILESPEC,
      PCP::PMAPI::PM_ERR_PMID_LOG,
      PCP::PMAPI::PM_ERR_INDOM_LOG,
      PCP::PMAPI::PM_ERR_INST_LOG,
      PCP::PMAPI::PM_ERR_NOPROFILE,
      PCP::PMAPI::PM_ERR_NOAGENT,
      PCP::PMAPI::PM_ERR_PERMISSION,
      PCP::PMAPI::PM_ERR_CONNLIMIT,
      PCP::PMAPI::PM_ERR_AGAIN,
      PCP::PMAPI::PM_ERR_ISCONN,
      PCP::PMAPI::PM_ERR_NOTCONN,
      PCP::PMAPI::PM_ERR_NEEDPORT,
      PCP::PMAPI::PM_ERR_NONLEAF,
      PCP::PMAPI::PM_ERR_TYPE,
      PCP::PMAPI::PM_ERR_THREAD,
      PCP::PMAPI::PM_ERR_NOCONTAINER,
      PCP::PMAPI::PM_ERR_BADSTORE,
      PCP::PMAPI::PM_ERR_TOOSMALL,
      PCP::PMAPI::PM_ERR_TOOBIG,
      PCP::PMAPI::PM_ERR_FAULT,
      PCP::PMAPI::PM_ERR_PMDAREADY,
      PCP::PMAPI::PM_ERR_PMDANOTREADY,
      PCP::PMAPI::PM_ERR_NYI,

      PCP::PMAPI::PMNS_LEAF_STATUS,
      PCP::PMAPI::PMNS_NONLEAF_STATUS,
    ].each do |constant|
      it "should be numeric" do
        expect(constant).to be_a_kind_of Integer
      end
    end
  end

  describe 'TODOS' do
    it 'replace all malloc() with ALLOC()'
    it 'check that memory is freed before we use rb_raise()'
  end

  describe 'integration tests' do

    let(:pmapi) { PCP::PMAPI.new(PCP::PMAPI::PM_CONTEXT_HOST, "localhost") }
    let(:test_archive) { File.expand_path(File.dirname(__FILE__) + '/../test_archive') }
    let(:pmapi_archive) { PCP::PMAPI.new(PCP::PMAPI::PM_CONTEXT_ARCHIVE, test_archive) }


    describe '#new' do
      it 'should construct without errors for a host context' do
        PCP::PMAPI.new(PCP::PMAPI::PM_CONTEXT_HOST, "localhost")
      end
      it 'should construct without errors for an archive context' do
        expect{pmapi_archive}.to_not raise_error
      end
      it 'should raise an error for invalid non-existent' do
        expect{PCP::PMAPI.new(PCP::PMAPI::PM_CONTEXT_ARCHIVE, 'archive_that_doesnt_exist')}.to raise_error PCP::PMAPI::Error
      end
    end

    describe '#pmGetContextHostName' do
      it 'should return the hostname of the created context' do
        expect(pmapi.pmGetContextHostName).to eq 'ryandesktop'
      end
    end

    describe '#pmGetPMNSLocation' do
      it 'returns the PMNS location' do
        expect(pmapi.pmGetPMNSLocation).to eq PCP::PMAPI::PMNS_REMOTE
      end
    end

    describe '#pmLoadNameSpace' do
      it 'should raise an error for non-existent files' do
        expect{pmapi.pmLoadNameSpace('some-namespace-that-doesnt-exist.txt')}.to raise_error PCP::PMAPI::Error
      end
    end

    describe '#pmUnloadNameSpace' do
      it 'should raise an error for non-existent files' do
        expect{pmapi.pmUnloadNameSpace}.to_not raise_error
      end
    end

    describe '#pmTrimNameSpace' do
      it 'should not raise an error' do
        expect{pmapi.pmTrimNameSpace}.to_not raise_error
      end
    end

    describe '#pmLookupName' do
      it 'should fetch a single metric PMID' do
        expect(pmapi.pmLookupName(['disk.all.read'])).to eq [{"disk.all.read"=>251658264}]
      end
      it 'should fetch multiple metric PMIDs' do
        expect(pmapi.pmLookupName(['disk.all.read', 'disk.all.write'])).to eq [{"disk.all.read"=>251658264}, {"disk.all.write"=>251658265}]
      end
      it 'should raise an error for unknown names' do
        expect{pmapi.pmLookupName(['something.that.doesnt.exist'])}.to raise_error PCP::PMAPI::NameError
      end
    end

    describe '#pmGetChildren' do
      it 'should return immediate children of the metric' do
        expect(pmapi.pmGetChildren('swap')).to eq ["pagesin", "pagesout", "in", "out", "free", "length", "used"]
      end
      it 'should raise an error for invalid metrics' do
        expect{pmapi.pmGetChildren('invalid.metric.name')}.to raise_error PCP::PMAPI::NameError
      end
      it 'should return an empty list for metrics that are leaf nodes' do
        expect(pmapi.pmGetChildren('swap.used')).to eq []
      end
    end

    describe '#pmGetChildrenStatus' do
      it 'should return the children and their status type' do
        expect(pmapi.pmGetChildrenStatus('swap')).to eq [{"pagesin"=>0}, {"pagesout"=>0}, {"in"=>0}, {"out"=>0}, {"free"=>0}, {"length"=>0}, {"used"=>0}]
      end
      it 'should return children with a status flag indicating they are non-leaf nodes' do
        expect(pmapi.pmGetChildrenStatus('disk')).to eq [{"dev"=>1}, {"all"=>1}, {"partitions"=>1}, {"dm"=>1}]
      end
      it 'should return an empty list for metrics that are leaf nodes' do
        expect(pmapi.pmGetChildrenStatus('swap.used')).to eq []
      end
      it 'should raise an error for invalid metrics' do
        expect{pmapi.pmGetChildrenStatus('invalid.metric.name')}.to raise_error PCP::PMAPI::NameError
      end
    end

    describe '#pmNameID' do
      it 'returns the name for a pmid' do
        expect(pmapi.pmNameID(251658264)).to eq 'disk.all.read'
      end
      it 'raises an error for invalid PMIDs' do
        expect{pmapi.pmNameID(123456)}.to raise_error PCP::PMAPI::NoAgentError
      end
    end

    describe '#pmNameAll' do
      it 'returns all the names for a PMID' do
        expect(pmapi.pmNameAll(121634817)).to eq ["sample.dupnames.pid_daemon", "sample.dupnames.daemon_pid", "sample.daemon_pid"]
      end
      it 'raises an error for invalid PMIDs' do
        expect{pmapi.pmNameAll(123456)}.to raise_error PCP::PMAPI::NoAgentError
      end
    end

    describe '#pmTraversePMNS' do
      it 'yields when traversing the PMNS namespace' do
        expect { |b| pmapi.pmTraversePMNS('swap', &b) }.to yield_successive_args("swap.pagesin", "swap.pagesout", "swap.in", "swap.out", "swap.free", "swap.length", "swap.used")
      end
      it 'raises an error for invalid metric IDs' do
        expect{ pmapi.pmTraversePMNS('some.invalid.name') }.to raise_error PCP::PMAPI::NameError
      end
    end

    describe "#pmLookupDesc" do
      it 'returns the descriptor for a PMID' do
        expect(pmapi.pmLookupDesc(251658264)).to eq PCP::PMAPI::PmDesc.new(251658264, 3, 4294967295, 1, PCP::PMAPI::PmUnits.new(0,0,1,0,0,0))
      end
      it 'raises an error for invalid metrics IDs' do
        expect{pmapi.pmLookupDesc(123456)}.to raise_error PCP::PMAPI::PMIDError
      end
    end

    # sample.many.int == PMID: 121634896 == Instance domain 121634824
    describe '#pmLookupInDom' do
      it 'returns the internal instance identifier given an instance domain and the instance name' do
        expect(pmapi.pmLookupInDom(121634824, "i-0")).to eq 0
      end
      it 'returns an error for unknown instance identifiers' do
        expect{pmapi.pmLookupInDom(121634824, "doesntexist")}.to raise_error PCP::PMAPI::InstError
      end
    end

    describe '#pmLookupInDomArchive' do
      it 'requires integration testing'
      it 'raises an error if not used in an archive context' do
        expect{pmapi.pmLookupInDomArchive(121634824, "i-0")}.to raise_error PCP::PMAPI::NotArchiveError
      end
    end

    describe '#pmNameInDom' do
      it 'should return the text name of an instance from an instance domain and and instance id' do
        expect(pmapi.pmNameInDom(121634824, 0)).to eq "i-0"
      end
      it 'should raise an error for invalid instance domains' do
        expect{pmapi.pmNameInDom(123, 1)}.to raise_error PCP::PMAPI::InDomError
      end
      it 'should raise an error for invalid instance ids' do
        expect{pmapi.pmNameInDom(121634824, 123)}.to raise_error PCP::PMAPI::InstError
      end
    end

    describe '#pmNameInDomArchive' do
      it 'requires integration testing'
      it 'raises an error if not used in an archive context' do
        expect{pmapi.pmNameInDomArchive(121634824, 0)}.to raise_error PCP::PMAPI::NotArchiveError
      end
    end

    describe '#pmGetInDom' do
      it 'returns a list of instance ids and names for an instance domain' do
        expect(pmapi.pmGetInDom(121634824)).to eq [{0=>"i-0"}, {1=>"i-1"}, {2=>"i-2"}, {3=>"i-3"}, {4=>"i-4"}]
      end
      it 'raises an error for invalid instance domains' do
        expect{pmapi.pmGetInDom(123456)}.to raise_error PCP::PMAPI::InDomError
      end
    end

    describe '#pmGetInDomArchive' do
      it 'requires integration testing'
      it 'raises an error if not used in an archive context' do
        expect{pmapi.pmGetInDomArchive(123456)}.to raise_error PCP::PMAPI::NotArchiveError
      end
    end

    describe '#pmWhichContext' do
      it 'returns the handle of the current context' do
        expect(pmapi.pmWhichContext).to be_a_kind_of Integer
      end
    end

    describe '#pmDestroyContext' do
      it 'should raise a Not Implemented error' do
        expect{pmapi.pmDestroyContext}.to raise_error NotImplementedError
      end
    end

    describe '#pmDupContext' do
      it 'should raise a Not Implemented error' do
        expect{pmapi.pmDupContext}.to raise_error NotImplementedError
      end
    end

    describe '#pmUseContext' do
      it 'should raise a Not Implemented error' do
        expect{pmapi.pmUseContext}.to raise_error NotImplementedError
      end
    end

    describe '#pmReconnectContext' do
      it 'reconnects the context if it is disconnected' do
        expect{pmapi.pmReconnectContext}.to_not raise_error
      end
    end

    describe '#pmDelProfile' do

      before do
        # Profile starts off as null, so add all instances first
        pmapi.pmAddProfile(121634824, [])
      end

      it 'removes the instance from being returned in a fetch' do
        pmapi.pmDelProfile(121634824, [0])

        expect(pmapi.pmFetch([121634896])).to_not have_the_value(0).for_pmid(121634896).and_instance(0)
      end

      it 'removes multiple instances from being returned in a fetch' do
        pmapi.pmDelProfile(121634824, [0,1])

        expect(pmapi.pmFetch([121634896])).to_not have_the_value(0).for_pmid(121634896).and_instance(0)
        expect(pmapi.pmFetch([121634896])).to_not have_the_value(1).for_pmid(121634896).and_instance(1)
      end

      it 'does not remove instances that arent specified' do
        pmapi.pmDelProfile(121634824, [0])

        expect(pmapi.pmFetch([121634896])).to have_the_value(1).for_pmid(121634896).and_instance(1)
      end


      it 'deletes all instances from an instance domain when given an empty list' do
        pmapi.pmDelProfile(121634824, [])

        expect(pmapi.pmFetch([121634896])).to_not have_the_value(0).for_pmid(121634896).and_instance(0)
        expect(pmapi.pmFetch([121634896])).to_not have_the_value(1).for_pmid(121634896).and_instance(1)
      end

      it 'removes all instances from all domains for a null instance domain' do
        pmapi.pmDelProfile(PCP::PMAPI::PM_INDOM_NULL, [])

        expect(pmapi.pmFetch([121634896])).to_not have_the_value(0).for_pmid(121634896).and_instance(0)
      end

    end

    describe '#pmAddProfile' do

      before do
        # Clear all instance domains
        pmapi.pmDelProfile(PCP::PMAPI::PM_INDOM_NULL, [])
      end

      it 'adds an instance to be returned' do
        pmapi.pmAddProfile(121634824, [2])

        expect(pmapi.pmFetch([121634896])).to have_the_value(2).for_pmid(121634896).and_instance(2)
      end

      it 'adds an multiple instances to be returned' do
        pmapi.pmAddProfile(121634824, [0,2])

        expect(pmapi.pmFetch([121634896])).to have_the_value(0).for_pmid(121634896).and_instance(0)
        expect(pmapi.pmFetch([121634896])).to have_the_value(2).for_pmid(121634896).and_instance(2)
      end

      it 'adds all instances back onto an instance domain' do
        pmapi.pmAddProfile(121634824, [])

        (0..4).each do |instance|
          expect(pmapi.pmFetch([121634896])).to have_the_value(instance).for_pmid(121634896).and_instance(instance)
        end
      end

      it 'adds all instances from all domains' do
        pmapi.pmAddProfile(PCP::PMAPI::PM_INDOM_NULL, [])

        (0..4).each do |instance|
          expect(pmapi.pmFetch([121634896])).to have_the_value(instance).for_pmid(121634896).and_instance(instance)
        end
      end
    end

    describe '#pmFetch' do
      describe 'the result structure' do
        it 'has the number of PMIDs in a result' do
          expect(pmapi.pmFetch([121634820]).numpmid).to eql 1
        end

        it 'has the date that the request was made' do
          expect(pmapi.pmFetch([121634820]).timestamp).to be_kind_of(Time)
        end
        it 'has the value set' do
          expect(pmapi.pmFetch([121634820]).vset).to eq [
            PCP::PMAPI::PmValueSet.new(121634820, PCP::PMAPI::PM_VAL_INSITU, [
              PCP::PMAPI::PmValue.new(-1, 42 )
            ])
          ]
        end
      end

      describe 'when querying multiple pmids and instances' do
        it 'returns multiple instances for a pmid' do
          # 121634896 == sample.many.int
          pm_result = pmapi.pmFetch([121634896])

          expect(pm_result).to have_the_value(0).for_pmid(121634896).and_instance(0)
          expect(pm_result).to have_the_value(1).for_pmid(121634896).and_instance(1)
          expect(pm_result).to have_the_value(2).for_pmid(121634896).and_instance(2)
          expect(pm_result).to have_the_value(3).for_pmid(121634896).and_instance(3)
          expect(pm_result).to have_the_value(4).for_pmid(121634896).and_instance(4)
        end

        it 'returns a pmResult-like hash for multiple PMIDs' do
          pm_result = pmapi.pmFetch([121634820, 121634896])

          expect(pm_result).to have_the_value(0).for_pmid(121634896).and_instance(0)
          expect(pm_result).to have_the_value(1).for_pmid(121634896).and_instance(1)
          expect(pm_result).to have_the_value(2).for_pmid(121634896).and_instance(2)
          expect(pm_result).to have_the_value(3).for_pmid(121634896).and_instance(3)
          expect(pm_result).to have_the_value(4).for_pmid(121634896).and_instance(4)

          expect(pm_result).to have_the_value(42).for_pmid(121634820).and_instance(-1)
        end
      end

      describe 'when called for pmids of different types' do
        it 'returns string results' do
          # 121634847== sample.string.hullo
          expect(pmapi.pmFetch([121634847])).to have_the_value("hullo world!").for_pmid(121634847).and_instance(-1)
        end
        it 'returns unsigned long results' do
          # 121634912 == sample.ulong.million
          expect(pmapi.pmFetch([121634912])).to have_the_value(1000000).for_pmid(121634912).and_instance(-1)
        end
        it 'returns signed long results' do
          # 121634829 == sample.long.million
          expect(pmapi.pmFetch([121634829])).to have_the_value(1000000).for_pmid(121634829).and_instance(-1)
        end
        it 'returns unsigned long long results' do
          # 121634917 == sample.ulonglong.million
          expect(pmapi.pmFetch([121634917])).to have_the_value(1000000).for_pmid(121634917).and_instance(-1)
        end
        it 'returns signed long long results' do
          # 121634839 == sample.longlong.million
          expect(pmapi.pmFetch([121634839])).to have_the_value(1000000).for_pmid(121634839).and_instance(-1)
        end
      end

      it 'might need to support other types like PM_TYPE_EVENT etc...'

    end

    describe '#pmFetchArchive' do
      it 'requires integration testing'

      it 'raises an error if not used in the correct context' do
        expect{pmapi.pmFetchArchive}.to raise_error PCP::PMAPI::NotArchiveError
      end
    end

    describe '#pmGetArchiveLabel' do
      it 'returns the archive label when used in a pmapi archive context' do
        expect(pmapi_archive.pmGetArchiveLabel).to include :ll_hostname => 'ryandesktop', :ll_magic => 1342514690, :ll_pid => 28015, :ll_start => kind_of(Time), :ll_tz => 'EST-11'
      end
      it 'raises an error if not used in the correct context' do
        expect{pmapi.pmGetArchiveLabel}.to raise_error PCP::PMAPI::NoContextError
      end
    end

    describe '#pmGetArchiveEnd' do
      it 'should return the time that the archive ends' do
        expect(pmapi_archive.pmGetArchiveEnd).to be_kind_of(Time)
      end
      it 'raises an error if not used in the correct context' do
        expect{pmapi.pmGetArchiveLabel}.to raise_error PCP::PMAPI::NoContextError
      end
    end

    describe '#pmFreeResult' do
      it 'should raise an error because it is not implemented' do
        expect{pmapi.pmFreeResult}.to raise_error NotImplementedError
      end
    end

    describe '#pmFreeHighResResult' do
      it 'should raise an error because it is not implemented' do
        expect{pmapi.pmFreeHighResResult}.to raise_error NotImplementedError
      end
    end

    describe '#pmExtractValue' do
      it 'should raise an error because it is not implemented' do
        expect{pmapi.pmExtractValue}.to raise_error NotImplementedError
      end
    end

    describe '#pmPrintValue' do
      it 'should raise an error because it is not implemented' do
        expect{pmapi.pmPrintValue}.to raise_error NotImplementedError
      end
    end

  end

end
