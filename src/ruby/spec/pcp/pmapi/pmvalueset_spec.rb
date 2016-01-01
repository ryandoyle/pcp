require 'spec_helper'

describe PCP::PMAPI::PmValueSet do

  let(:pm_value_set) { described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [pm_value]) }
  let(:pm_value) { PCP::PMAPI::PmValue.new(111, 222) }
  let(:pm_value2) { PCP::PMAPI::PmValue.new(333, 444) }


  describe '#new' do
    it 'should set the vlist to the PmValue' do
      pm_value_set = described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [pm_value])
      expect(pm_value_set.vlist).to eq [pm_value]
    end

    it 'should set the vlist for more than one PmValue' do
      pm_value_set = described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [pm_value, pm_value2])
      expect(pm_value_set.vlist).to eq [pm_value, pm_value2]
    end

    it 'should raise an error for non-numeric PMIDs' do
      expect{described_class.new("not a number", PCP::PMAPI::PM_VAL_INSITU, [pm_value])}.to raise_error TypeError
    end

    it 'should raise an error if the vlist is not an array' do
      expect{described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, pm_value)}.to raise_error ArgumentError
    end

    it 'should raise an error if the vlist array contains non-PmValue types' do
      expect{described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [1])}.to raise_error ArgumentError
    end

    it 'should raise an error if the vlist contains any non-PmValue types' do
      expect{described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [pm_value, 1])}.to raise_error ArgumentError
    end

    it 'should raise an error for non-numeric value formats' do
      expect{described_class.new(123, "not a number", [])}.to raise_error TypeError
    end

  end

  describe '#pmid' do
    it 'should return the PMID' do
      expect(pm_value_set.pmid).to eq 123
    end
  end

  describe 'valfmt' do
    it 'should return the value format for the pmvalues' do
      expect(pm_value_set.valfmt).to eq PCP::PMAPI::PM_VAL_INSITU
    end
  end

  describe '#vlist' do
    it 'should return a list of PmValue(s)' do
      pm_value_set = described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [pm_value])

      expect(pm_value_set.vlist).to eq [pm_value]
    end
  end

  describe '#numval' do
    it 'return the number of values' do
      expect(pm_value_set.numval).to eq 1
    end
  end

  describe '#==' do
    it 'is true if pmid, valfmt and vlist are all the same' do
      other = described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [pm_value])
      expect(pm_value_set).to eq other
    end

    it 'is false if the pmid is different' do
      other = described_class.new(456, PCP::PMAPI::PM_VAL_INSITU, [pm_value])
      expect(pm_value_set).to_not eq other
    end

    it 'is false if the valfmt is different' do
      other = described_class.new(123, PCP::PMAPI::PM_VAL_DPTR, [pm_value])
      expect(pm_value_set).to_not eq other
    end

    it 'is false if the vlist is different' do
      other = described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [])
      expect(pm_value_set).to_not eq other
    end
  end

  describe 'going from Ruby object to C' do

    let(:pm_value_set) { described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [pm_value, pm_value2]) }
    let(:pm_value) { PCP::PMAPI::PmValue.new(111, 222) }
    let(:pm_value2) { PCP::PMAPI::PmValue.new(333, 444) }

    it 'should contain the pmid' do
      expect(pmvalueset_get_pmid(pm_value_set)).to eq 123
    end

    it 'should contain the value format' do
      expect(pmvalueset_get_valfmt(pm_value_set)).to eq PCP::PMAPI::PM_VAL_INSITU
    end

    it 'should contain the number of values' do
      expect(pmvalueset_get_numval(pm_value_set)).to eq 2
    end

    it 'should contain the first PmValue' do
      expect(pmvalueset_get_pmvalue_for_insitu(pm_value_set, 0)).to eq 222
    end

    it 'should contain the second PmValue' do
      expect(pmvalueset_get_pmvalue_for_insitu(pm_value_set, 1)).to eq 444
    end

  end

end