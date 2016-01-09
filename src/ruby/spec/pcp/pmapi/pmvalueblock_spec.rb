require 'spec_helper'

describe PCP::PMAPI::PmValueBlock do

  describe '#new' do
    it 'should construct correctly for a signed 64 bit int' do
      described_class.new(-123, PCP::PMAPI::PM_TYPE_64)
    end
    it 'should construct correctly for a an unsigned 64 bit int' do
      described_class.new(123, PCP::PMAPI::PM_TYPE_U64)
    end
    it 'should construct correctly for a float' do
      described_class.new(123.456, PCP::PMAPI::PM_TYPE_FLOAT)
    end
    it 'should construct correctly for a double' do
      described_class.new(-123.456, PCP::PMAPI::PM_TYPE_DOUBLE)
    end
    it 'should construct correctly for a string' do
      described_class.new("hello", PCP::PMAPI::PM_TYPE_STRING)
    end
    it 'should raise an error for values with incorrect types' do
      expect{described_class.new("hello", PCP::PMAPI::PM_TYPE_64)}.to raise_error TypeError
    end
  end

  describe '#vbuf' do
    it 'should return the value for a signed 64 bit int' do
      pm_value_block = described_class.new(-123123123123, PCP::PMAPI::PM_TYPE_64)
      expect(pm_value_block.vbuf).to eq -123123123123
    end
    it 'should return the value for an unsigned 64 bit int' do
      pm_value_block = described_class.new(123123123123, PCP::PMAPI::PM_TYPE_U64)
      expect(pm_value_block.vbuf).to eq 123123123123
    end
    it 'should return the value for a float' do
      pm_value_block = described_class.new(123.456, PCP::PMAPI::PM_TYPE_FLOAT)
      expect(pm_value_block.vbuf).to be_as_close_as_possible_to 123.456
    end
    it 'should return the value for a double' do
      pm_value_block = described_class.new(123123123.456, PCP::PMAPI::PM_TYPE_DOUBLE)
      expect(pm_value_block.vbuf).to eq 123123123.456
    end
    it 'should return the value for a string' do
      pm_value_block = described_class.new('aaaaa', PCP::PMAPI::PM_TYPE_STRING)
      expect(pm_value_block.vbuf).to eq 'aaaaa'
    end
  end

  describe '#vlen' do
    it 'should return the length of the value block header + vbuf for a signed 64 bit int' do
      pm_value_block = described_class.new(-123123123123, PCP::PMAPI::PM_TYPE_64)
      expect(pm_value_block.vlen).to eq PCP::PMAPI::PM_VAL_HDR_SIZE + 8
    end
    it 'should return the length of the value block header + vbuf for an unsigned 64 bit int' do
      pm_value_block = described_class.new(123123123123, PCP::PMAPI::PM_TYPE_U64)
      expect(pm_value_block.vlen).to eq PCP::PMAPI::PM_VAL_HDR_SIZE + 8
    end
    it 'should return the length of the value block header + vbuf for a float' do
      pm_value_block = described_class.new(123.456, PCP::PMAPI::PM_TYPE_FLOAT)
      expect(pm_value_block.vlen).to eq PCP::PMAPI::PM_VAL_HDR_SIZE + 4
    end
    it 'should return the length of the value block header + vbuf for a double' do
      pm_value_block = described_class.new(123.456, PCP::PMAPI::PM_TYPE_DOUBLE)
      expect(pm_value_block.vlen).to eq PCP::PMAPI::PM_VAL_HDR_SIZE + 8
    end
    it 'should return the length of the value block header + vbuf for a string (with a NULL-terminator)' do
      pm_value_block = described_class.new("aaaaa", PCP::PMAPI::PM_TYPE_STRING)
      expect(pm_value_block.vlen).to eq PCP::PMAPI::PM_VAL_HDR_SIZE + 6
    end
  end

  describe '#vtype' do
    it 'return the type for a signed 64 bit int' do
      pm_value_block = described_class.new(-123123123123, PCP::PMAPI::PM_TYPE_64)
      expect(pm_value_block.vtype).to eq PCP::PMAPI::PM_TYPE_64
    end
    it 'return the type for an unsigned 64 bit int' do
      pm_value_block = described_class.new(123123123123, PCP::PMAPI::PM_TYPE_U64)
      expect(pm_value_block.vtype).to eq PCP::PMAPI::PM_TYPE_U64
    end
    it 'return the type for a float' do
      pm_value_block = described_class.new(123.456, PCP::PMAPI::PM_TYPE_FLOAT)
      expect(pm_value_block.vtype).to eq PCP::PMAPI::PM_TYPE_FLOAT
    end
    it 'return the type for a double' do
      pm_value_block = described_class.new(123.456, PCP::PMAPI::PM_TYPE_DOUBLE)
      expect(pm_value_block.vtype).to eq PCP::PMAPI::PM_TYPE_DOUBLE
    end
    it 'return the type for a string' do
      pm_value_block = described_class.new('hello', PCP::PMAPI::PM_TYPE_STRING)
      expect(pm_value_block.vtype).to eq PCP::PMAPI::PM_TYPE_STRING
    end
  end

  describe 'inspect' do
    it 'should return a string representation of the state of the object' do
      pm_value_block = described_class.new(123, PCP::PMAPI::PM_TYPE_64)
      expect(pm_value_block.inspect).to eq '#<PCP::PMAPI::PmValueBlock: vlen=12 vtype=2 vbuf=123>'
    end
  end

  describe '==' do
    it 'is true for signed 64 bit ints of the same value' do
      instance1 = described_class.new(-123123123123, PCP::PMAPI::PM_TYPE_64)
      instance2 = described_class.new(-123123123123, PCP::PMAPI::PM_TYPE_64)

      expect(instance2).to eq instance1
    end
    it 'is true for unsigned 64 bit ints of the same value' do
      instance1 = described_class.new(123123123123, PCP::PMAPI::PM_TYPE_U64)
      instance2 = described_class.new(123123123123, PCP::PMAPI::PM_TYPE_U64)

      expect(instance2).to eq instance1
    end
    it 'is true for floats of the same value' do
      instance1 = described_class.new(123.456, PCP::PMAPI::PM_TYPE_FLOAT)
      instance2 = described_class.new(123.456, PCP::PMAPI::PM_TYPE_FLOAT)

      expect(instance2).to eq instance1
    end
    it 'is true for doubles of the same value' do
      instance1 = described_class.new(123.456, PCP::PMAPI::PM_TYPE_DOUBLE)
      instance2 = described_class.new(123.456, PCP::PMAPI::PM_TYPE_DOUBLE)

      expect(instance2).to eq instance1
    end
    it 'is true for strings of the same value' do
      instance1 = described_class.new('aaaa', PCP::PMAPI::PM_TYPE_STRING)
      instance2 = described_class.new('aaaa', PCP::PMAPI::PM_TYPE_STRING)

      expect(instance2).to eq instance1
    end
    it 'is false for different object classes' do
      instance1 = described_class.new('aaaa', PCP::PMAPI::PM_TYPE_STRING)
      instance2 = 'aaaa'

      expect(instance2).to_not eq instance1
    end
    it 'is false for the same object class but different values' do
      instance1 = described_class.new('aaaa', PCP::PMAPI::PM_TYPE_STRING)
      instance2 = described_class.new('bbbb', PCP::PMAPI::PM_TYPE_STRING)

      expect(instance2).to_not eq instance1
    end
    it 'is false for the same object with the same value but different types' do
      instance1 = described_class.new(123, PCP::PMAPI::PM_TYPE_U64)
      instance2 = described_class.new(123, PCP::PMAPI::PM_TYPE_64)

      expect(instance2).to_not eq instance1
    end
  end


  describe 'converting Ruby object to C' do

    let(:pm_value_block) { pm_value_block = described_class.new('aaaa', PCP::PMAPI::PM_TYPE_STRING) }

    it 'should return the vbuf' do
      expect(pmvalueblock_get_vbuf(pm_value_block)).to eq 'aaaa'
    end

    it 'should return the vlen' do
      expect(pmvalueblock_get_vlen(pm_value_block)).to eq 9
    end

    it 'should return the vtype' do
      expect(pmvalueblock_get_vtype(pm_value_block)).to eq PCP::PMAPI::PM_TYPE_STRING
    end
  end


end