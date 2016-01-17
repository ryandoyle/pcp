require 'forwardable'

require 'pcp_native'
require 'pcp/pmapi/pmunits'
require 'pcp/pmapi/pmdesc'
require 'pcp/pmapi/pmvalue'
require 'pcp/pmapi/pmvalueset'
require 'pcp/pmapi/pmvalueblock'
require 'pcp/pmapi/pmresult'
require 'pcp/pmapi/pmloglabel'

module PCP
  class PMAPI

    extend Forwardable

    def initialize(metric_source, metric_source_argument)
      unless(metric_source_argument.class == String || metric_source_argument == nil)
        raise TypeError, 'Metric source argument should be String or Nil' unless metric_source_argument
      end
      pmNewContext(metric_source, metric_source_argument)
    end

    alias_method :pmLoadASCIINameSpace, :pmLoadNameSpace

    def_delegator self, :pmIDStr

  end
end
