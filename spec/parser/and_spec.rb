def test_case
{"RawParseTree"=>[:and, [:vcall, :a], [:vcall, :b]],
 "Ruby"=>"(a and b)",
 "RubyParser"=>
  s(:and, s(:call, nil, :a, s(:arglist)), s(:call, nil, :b, s(:arglist)))}
end
