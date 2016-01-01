require 'pcp/pmapi'

module PCP
  class PMAPI
    class PmResult

      def inspect
        "#<#{self.class.name}: timestamp=#{timestamp} numpmid=#{numpmid} vset={vset}>"
      end

    end
  end
end
