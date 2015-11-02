#!/usr/bin/env ruby

pwd = `pwd`.strip
dir = pwd + '/a_subdir'

File.open('profile.cfg', 'w') do |f|
  f.puts '[copy]'
  f.puts "#{dir}/a"
  f.puts "#{dir}/a"
  f.puts "#{dir}/c"
  f.puts "#{dir}/c"
  f.puts "#{dir}/e"
  f.puts "#{dir}/e"
  f.puts "#{dir}/numdir/"
  f.puts '[clean]'
  f.puts "#{dir}/"
end

