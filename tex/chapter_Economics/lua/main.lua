function sum_arr(arr)
	local s = 0
	for i=1, #arr do
		s = s + arr[i]
	end
	return s
end

function copy_arr(from, to)
	for i=1, #from do
		to[i] = from[i]
	end
end

function write_arr(arr, name)
	if name == nil then
		name = ''
	end
	io.write(name)
	for i=1, #arr do
		io.write(arr[i], '  ')
	end
	io.write('\n')
end

function write_mat(mat)
	for i=1, #mat do
		write_arr(mat[i])
		io.write('\n')
	end
end

N = 7 --число экспертов
n = 4 --число критериев
table1 = {
			{4, 6, 4, 6, 6, 5, 6}, --Швидкодія мови програмування 
			{3, 2, 2,2, 3, 1, 3 }, --Об’єм оперативної пам’яті Мб 
			{2, 2, 3, 3, 1, 3, 2 }, --Час обробки запиту користувача Мс 
			{6, 5, 6, 4, 5, 6, 4 }  --Кількість запитів - 
		}
rangSum = {0,0,0,0} 	--R_i
allRangSum = 0 			--R
for i=1, #table1 do
	rangSum[i] = sum_arr(table1[i])
	-- print(rangSum[i])
	allRangSum = allRangSum + rangSum[i]
end
allAvgSum = allRangSum / 4 	--T

rangDelta = {0,0,0,0}		--\delta_i
deltaSquares = 0			--S
passDelta = 0
for i=1,#rangDelta do
	rangDelta[i] = rangSum[i] - allAvgSum
	deltaSquares = deltaSquares + (math.pow(rangDelta[i],2))
	passDelta  = passDelta + rangDelta[i]
	-- print('r',rangDelta[i])
end

if passDelta ~= 0 then
	print ('ERROR: passDelta ==',passDelta)
end
--print(deltaSquares)

W = (12*deltaSquares)/(math.pow(N,2)*(math.pow(n,3) - n))
if W < 0.67 then
	print 'ERROR: коэфициент ранжирования не подходит'
end

--дальше попарно сравниваем элементы
a = {}
for i=1, #table1 do 		--по X_i
	a[i] = {}
	for j=1, #(table1) do
		a[i][j] = 0
	end
	
	for j=1,#table1 do 		--по X_j
		x = 0	
		for z=1, #(table1[j]) do
			if table1[i][z] > table1[j][z] then
				x = x + 1
			end
			if table1[i][z] < table1[j][z] then
				x = x - 1
			end
			if table1[i][z] > table1[j][z] then
				x = x + 0
			end
		end

		if x > 0 then
			a[i][j] = 1.5	
		elseif x == 0 then
			a[i][j] = 1.0
		elseif x < 0 then
			a[i][j] = 0.5
		end
		-- io.write(a[i][j], '  ')
	end
	-- io.write('\n')
end

write_mat(a)

-- Для кожного параметра зробимо розрахунок вагомості Kві за наступними 

kWeight = {}
kWeight_old = {}
b_old = {}
b = {}
for i=1, #a do
	b[i] = 1
	b_old[i] = 1
	kWeight[i] = 0
end

END = false
repeat
	io.write('iter_b: ')
	copy_arr(b,b_old)
	copy_arr(kWeight, kWeight_old)
	for i=1, #a do
		b[i] = 0
		for j=1, #a do
			b[i] = b[i] + (a[i][j]*b_old[j])
		end
		io.write(b[i], '  ')
	end
	io.write('\n')

	-- io.write('\n: k_относительный')
	for i=1, #a do
		local bsum = sum_arr(b)
		kWeight[i] = b[i] / bsum
		-- io.write(kWeight[i], '  ')
	end
	local local_end = true
	for i=1, #kWeight do
		if math.abs(kWeight[i] - kWeight_old[i]) > 0.02 then
			local_end = false
		end
	end

	write_arr(kWeight, "iter_kweight: ")
	io.write("\n")

	if local_end then 
		END = true
	end
until END == true

--4.3 Аналіз рівня якості варіантів реалізації функцій 