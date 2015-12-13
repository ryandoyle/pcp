require 'pcp/pmapi'

describe PCP::PMAPI::PmValueSet do

  let(:pm_value_set) { described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [pm_value]) }
  let(:pm_value) { double('PCP::PMAPI::PmValue') }

  before do
    allow(pm_value).to receive(:class).and_return PCP::PMAPI::PmValue
  end

  describe '#pmid' do
    it 'should return the PMID' do
      expect(pm_value_set.pmid).to eq 123
    end
  end

  describe '#pmid=' do
    it 'should set the pmid' do
      pm_value_set.pmid = 456
      expect(pm_value_set.pmid).to eq 456
    end

    it 'should raise an error for non-numeric PMIDs' do
      expect{pm_value_set.pmid = "Not a number"}.to raise_error TypeError
    end
  end

  describe 'valfmt' do
    it 'should return the value format for the pmvalues' do
      expect(pm_value_set.valfmt).to eq PCP::PMAPI::PM_VAL_INSITU
    end
  end

  describe 'valfmt=' do
    it 'should set the valfmt' do
      pm_value_set.valfmt = PCP::PMAPI::PM_VAL_DPTR
      expect(pm_value_set.valfmt).to eq PCP::PMAPI::PM_VAL_DPTR
    end

    it 'should raise an error for non-numeric value formats' do
      expect{pm_value_set.valfmt = "Not a number"}.to raise_error TypeError
    end
  end

  describe '#vlist' do
    it 'should return a list of PmValue(s)' do
      pm_value_set = described_class.new(123, PCP::PMAPI::PM_VAL_INSITU, [pm_value])

      expect(pm_value_set.vlist).to eq [pm_value]
    end
  end

  describe '#vlist=' do
    it 'should set the vlist to the PmValue' do
      pm_value_set.vlist = [pm_value]
      expect(pm_value_set.vlist).to eq [pm_value]
    end

    it 'should raise an error if the vlist is not an array' do
      expect{pm_value_set.vlist = pm_value}.to raise_error ArgumentError
    end

    it 'should raise an error if the vlist array contains non-PmValue types' do
      expect{pm_value_set.vlist = [1]}.to raise_error ArgumentError
    end

    it 'should raise an error if the vlist contains any non-PmValue types' do
      expect{pm_value_set.vlist = [pm_value, 1]}.to raise_error ArgumentError
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

end