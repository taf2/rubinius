def test_case
{"RawParseTree"=>[:if, [:match, [:lit, /x/]], [:lit, 1], nil],
 "Ruby"=>"1 if /x/",
 "ParseTree"=>s(:if, s(:match, s(:lit, /x/)), s(:lit, 1), nil)}
end