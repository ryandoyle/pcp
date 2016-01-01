require 'spec_helper'

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

  describe 'converting Ruby object to C' do
    let(:pm_units) { described_class.new(-1,-2,-3,4,5,6) }

    it 'should contain the correct dimSpace' do
      expect(pmunits_get_dimSpace(pm_units)).to eq -1
    end

    it 'should contain the correct dimTime' do
      expect(pmunits_get_dimTime(pm_units)).to eq -2
    end

    it 'should contain the correct dimCount' do
      expect(pmunits_get_dimCount(pm_units)).to eq -3
    end

    it 'should contain the correct scaleSpace' do
      expect(pmunits_get_scaleSpace(pm_units)).to eq 4
    end

    it 'should contain the correct scaleTime' do
      expect(pmunits_get_scaleTime(pm_units)).to eq 5
    end

    it 'should contain the correct scaleCount' do
      expect(pmunits_get_scaleCount(pm_units)).to eq 6
    end
  end

end