require 'pcp/pmapi'


describe PCP::PMAPI::PmDesc do

  let(:pm_units) { PCP::PMAPI::PmUnits.new(0,0,0,0,0,0) }
  let(:pm_desc) { described_class.new(123, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units) }

  describe '#pmid' do
    it 'returns the PMID' do
      expect(pm_desc.pmid).to eq 123
    end
  end

  describe '#type' do
    it 'returns the type of the metric' do
      expect(pm_desc.type).to eq PCP::PMAPI::PM_TYPE_64
    end
  end

  describe '#indom' do
    it 'returns the instance domain' do
      expect(pm_desc.indom).to eq 445566
    end
  end

  describe '#sem' do
    it 'returns the semantics' do
      expect(pm_desc.sem).to eq PCP::PMAPI::PM_SEM_COUNTER
    end
  end

  describe '#units' do
    it 'returns the pmUnits instance associated with this pmDesc instance' do
      expect(pm_desc.units).to eq pm_units
    end
  end

  describe '#new' do
    it 'should raise an error if constructed with a non-numeric pmid' do
      expect{described_class.new(nil,0,0,0,pm_units)}.to raise_error TypeError
    end
    it 'should raise an error if constructed with a non-numeric type' do
      expect{described_class.new(0,nil,0,0,pm_units)}.to raise_error TypeError
    end
    it 'should raise an error if constructed with a non-numeric indom' do
      expect{described_class.new(0,0,nil,0,pm_units)}.to raise_error TypeError
    end
    it 'should raise an error if constructed with a non-numeric semantics' do
      expect{described_class.new(0,0,0,nil,pm_units)}.to raise_error TypeError
    end
    it 'should raise an error if constructed with invalid units' do
      expect{described_class.new(0,0,0,0,0)}.to raise_error TypeError
    end
  end

  describe '#==' do
    it 'should be true for PmDesc classes that hold the same values' do
      instance_1 = described_class.new(123, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units)
      instance_2 = described_class.new(123, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units)

      expect(instance_1).to eq instance_2
    end
    it 'should be false for instances that do not hold the same pmid' do
      instance_1 = described_class.new(123, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units)
      instance_2 = described_class.new(456, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units)

      expect(instance_1).to_not eq instance_2
    end
    it 'should be false for instances that do not hold the same type' do
      instance_1 = described_class.new(123, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units)
      instance_2 = described_class.new(123, PCP::PMAPI::PM_TYPE_U64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units)

      expect(instance_1).to_not eq instance_2
    end
    it 'should be false for instances that do not hold the same indom' do
      instance_1 = described_class.new(123, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units)
      instance_2 = described_class.new(123, PCP::PMAPI::PM_TYPE_64, 111111, PCP::PMAPI::PM_SEM_COUNTER, pm_units)

      expect(instance_1).to_not eq instance_2
    end
    it 'should be false for instances that do not hold the same semantics' do
      instance_1 = described_class.new(123, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units)
      instance_2 = described_class.new(123, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_INSTANT, pm_units)

      expect(instance_1).to_not eq instance_2
    end
    it 'should be false for instances that do not hold the same units' do
      pm_units_2 = PCP::PMAPI::PmUnits.new(1,0,0,0,0,0)
      instance_1 = described_class.new(123, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units)
      instance_2 = described_class.new(123, PCP::PMAPI::PM_TYPE_64, 445566, PCP::PMAPI::PM_SEM_COUNTER, pm_units_2)

      expect(instance_1).to_not eq instance_2
    end
  end

end