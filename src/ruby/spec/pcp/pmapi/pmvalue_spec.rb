require 'pcp/pmapi'


describe PCP::PMAPI::PmValue do

  let(:pm_value) { described_class.new(111, 222) }

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

  describe '#inst=' do
    it 'should raise an error if set with an invalid type' do
      expect{pm_value.inst = ["invalid"]}.to raise_error TypeError
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

end