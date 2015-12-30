require 'pcp/pmapi'

module PCP
  class PMAPI
    class PmValueSet

      def initialize(pmid, valfmt, vlist)
        self.pmid = pmid
        self.valfmt = valfmt
        self.vlist = vlist
      end

      def numval
        @vlist.length
      end

      def vlist
        @vlist
      end

      def vlist=(vlist)
        raise ArgumentError, "vlist must be an array" unless vlist.class == Array
        raise ArgumentError, "vlist must contain only PCP::PMAPI::PmValue instances" unless vlist.all? {|e| e.class == PCP::PMAPI::PmValue }
        @vlist = vlist
      end

      def ==(other)
        pmid == other.pmid && valfmt == other.valfmt && numval == other.numval && vlist == other.vlist
      end

      def inspect
        "#<#{self.class.name}: pmid=#{pmid} numval=#{numval} valfmt=#{valfmt} vlist=#{vlist}>"
      end

    end
  end
end