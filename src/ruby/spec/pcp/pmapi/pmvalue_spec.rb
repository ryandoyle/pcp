require 'spec_helper'


describe PCP::PMAPI::PmValue do

  let(:pm_value) { described_class.new(111, 222) }

  describe '#new' do
    it 'should construct correctly for Fixnum values' do
      expect{described_class.new(111, 222)}.to_not raise_error
    end
    it 'should construct correctly for PmValueBlock values' do
      expect{described_class.new(111, PCP::PMAPI::PmValueBlock.new('hello', PCP::PMAPI::PM_TYPE_STRING))}.to_not raise_error
    end
    it 'should raise an error if constructed with neither a Fixnum or PmValueBlock' do
      expect{described_class.new(111, "hello")}.to raise_error ArgumentError
    end
    it 'should raise an error if constructed with a number too big to fit insitu' do
      expect{described_class.new(111, 123123123131)}.to raise_error RangeError
    end
  end

  describe '#value' do
    it 'should return the value' do
      expect(pm_value.value).to eq 222
    end
  end

  describe '#inst' do
    it 'should return the instance' do
      expect(pm_value.inst).to eq 111
    end
  end


  describe '#==' do
    it 'should be true for PmValue classes that hold the same instance and value' do
      instance_1 = described_class.new(123, 222)
      instance_2 = described_class.new(123, 222)

      expect(instance_1).to eq instance_2
    end

    it 'should be false if the instance is different' do
      instance_1 = described_class.new(123, 222)
      instance_2 = described_class.new(444, 222)

      expect(instance_1).to_not eq instance_2
    end

    it 'should be false if the value is different' do
      instance_1 = described_class.new(123, 222)
      instance_2 = described_class.new(123, 444)

      expect(instance_1).to_not eq instance_2
    end
  end

  describe 'converting Ruby objects to C' do
    it 'should have the inst' do
      pm_value = described_class.new(123, 222)

      expect(pmvalue_get_inst(pm_value)).to eq 123
    end

    it 'should have a value for insitu values' do
      pm_value = described_class.new(123, 222)

      expect(pmvalue_get_insitu_value(pm_value)).to eq 222
    end

    it 'should have a value for pmvalueblock values' do
      pm_value_block = PCP::PMAPI::PmValueBlock.new('hello', PCP::PMAPI::PM_TYPE_STRING)
      pm_value = described_class.new(123, pm_value_block)

      expect(pmvalue_get_pmvalueblock_vbuf_as_string(pm_value)).to eq 'hello'
    end
  end

end