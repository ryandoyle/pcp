require 'pcp/pmapi/pmunits'

describe PCP::PMAPI::PmUnits do

  describe 'dimSpace' do
    it 'should return the space dimension' do
      expect(described_class.new(1,0,0,0,0,0).dimSpace).to eq 1
    end
  end

  describe 'dimTime' do
    it 'should return the time dimension' do
      expect(described_class.new(0,1,0,0,0,0).dimTime).to eq 1
    end
  end

  describe 'dimCount' do
    it 'should return the count dimension' do
      expect(described_class.new(0,0,1,0,0,0).dimCount).to eq 1
    end
  end

  describe 'scaleSpace' do
    it 'should return the scaling of space' do
      expect(described_class.new(0,0,0,PCP::PMAPI::PM_SPACE_MBYTE,0,0).scaleSpace).to eq PCP::PMAPI::PM_SPACE_MBYTE
    end
  end

  describe 'scaleTime' do
    it 'should return the scaling of time' do
      expect(described_class.new(0,0,0,0,PCP::PMAPI::PM_TIME_SEC,0).scaleTime).to eq PCP::PMAPI::PM_TIME_SEC
    end
  end

  describe 'scaleCount' do
    it 'should return the scaling of counts' do
      expect(described_class.new(0,0,0,0,0,2).scaleCount).to eq 2
    end
  end

  describe '#==' do
    it 'is true for objects that share the same fields' do
      expect(described_class.new(1,2,3,4,5,6)).to eq described_class.new(1,2,3,4,5,6)
    end

    it 'is false for objects that do not share all the same fields' do
      expect(described_class.new(1,2,3,4,5,6)).to_not eq described_class.new(1,2,3,4,5,1)
    end
  end


end