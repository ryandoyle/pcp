require 'spec_helper'

describe PCP::PMAPI::PmResult do

    let(:time) { Time.utc(2000,"jan",1,20,15,1) }
    let(:pm_result) { described_class.new(time, [vset1, vset2]) }

    let(:vset1) { PCP::PMAPI::PmValueSet.new(123, PCP::PMAPI::PM_VAL_INSITU, [pm_value1]) }
    let(:vset2) { PCP::PMAPI::PmValueSet.new(456, PCP::PMAPI::PM_VAL_INSITU, [pm_value2]) }

    let(:pm_value1) { PCP::PMAPI::PmValue.new(111,222) }
    let(:pm_value2) { PCP::PMAPI::PmValue.new(333,444) }

  describe '#new' do
    it 'should not raise an error for correctly constructed instances' do
      expect{pm_result}.to_not raise_error
    end
    it 'should raise an error if the vset is not an array' do
      expect{described_class.new(time, 123)}.to raise_error ArgumentError
    end
    it 'should raise an error if the vset is not an array PmValueSet' do
      expect{described_class.new(time, [123])}.to raise_error ArgumentError
    end
    it 'should raise an error if the vset does not contain all PmValueSet' do
      expect{described_class.new(time, [vset1, 123])}.to raise_error ArgumentError
    end
  end

  describe '#timestamp' do
    it 'should return a the time that the PmResult was created with' do
      expect(pm_result.timestamp).to eq time
    end
  end

  describe '#numpmid' do
    it 'should return the number of vsets (which happen to be one per PMID)' do
      expect(pm_result.numpmid).to eq 2
    end
    it 'should return the number of vsets even if constructed with no vsets' do
      expect(described_class.new(time, []).numpmid).to eq 0
    end
  end

  describe '#vset' do
    it 'should return an array of PmValueSet associated with this PmResult' do
      expect(pm_result.vset).to eq [vset1, vset2]
    end
  end

  describe '#==' do
    let(:other_time) { Time.utc(2000,"jan",1,20,15,1) }
    let(:other_pm_result) { described_class.new(other_time, [other_vset1, other_vset2]) }

    let(:other_vset1) { PCP::PMAPI::PmValueSet.new(123, PCP::PMAPI::PM_VAL_INSITU, [other_pm_value1]) }
    let(:other_vset2) { PCP::PMAPI::PmValueSet.new(456, PCP::PMAPI::PM_VAL_INSITU, [other_pm_value2]) }
    let(:other_vset3) { PCP::PMAPI::PmValueSet.new(456, PCP::PMAPI::PM_VAL_INSITU, []) }

    let(:other_pm_value1) { PCP::PMAPI::PmValue.new(111,222) }
    let(:other_pm_value2) { PCP::PMAPI::PmValue.new(333,444) }

    it 'is true when compared to a PmResult that holds the same data' do
      expect(pm_result == other_pm_result).to be true
    end

    it 'is false when compared against a non PmResult type' do
      expect(pm_result == 123).to be false
    end

    it 'is false when the timestamp is different' do
      expect(pm_result == described_class.new(Time.utc(2001,"jan",1,20,15,1), [other_vset1, other_vset2])).to be false
    end

    it 'is false when the numpmid is different' do
      expect(pm_result == described_class.new(other_time, [other_vset1])).to be false
    end

    it 'is false when the vset is different' do
      expect(pm_result == described_class.new(other_time, [other_vset1, other_vset3])).to be false
    end

  end

  describe 'converting Ruby object to C' do
    it 'should contain the correct time' do
      expect(pmresult_get_timestamp(pm_result)).to eq Time.utc(2000,"jan",1,20,15,1)
    end

    it 'should contain the correct number of vsets for an array of vsets' do
      expect(pmresult_get_numpmid(pm_result)).to eq 2
    end

    it 'should contain a vset with the correct PMID' do
      expect(pmresult_get_pmid_for_nth_vset(pm_result, 0)).to eq 123
    end

    it 'should contain the other vset with the correct PMID' do
      expect(pmresult_get_pmid_for_nth_vset(pm_result, 1)).to eq 456
    end

  end
end