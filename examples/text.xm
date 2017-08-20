find_index = @(text, i, predicate)
	n = text.size(
	for ; i < n; i = i + 1: if predicate(text.code_at(i)): break
	i
$parse = @(text, delimiter, callback)
	not_delimiter = @(x) !delimiter(x
	i = 0
	while true
		i = find_index(text, i, not_delimiter
		if i >= text.size(): break
		j = find_index(text, i, delimiter
		callback(text.substring(i, j - i
		i = j
