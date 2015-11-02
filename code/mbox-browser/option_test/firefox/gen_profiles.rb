#!/usr/bin/env ruby

def usage
  puts "#{__FILE__} [profile_name]"
  exit
end

if ARGV.length < 1
  usage
end

dir = ARGV[0]

begin
  Dir.open(dir)
rescue
  usage
end

pwd = `pwd`.strip
find_all = `find #{dir}`.split "\n"

new_list = find_all.select{|x| x != dir}.map{|x| "#{pwd}/#{x}"}

File.open("#{dir}.cfg", 'w') do |f|
  f.puts "# first line is src file, second line is destination path"
  f.puts '[copy]'
  new_list.each do |x|
    f.puts x
    f.puts '/home/blue9057/.mozilla/firefox/' + x.split("#{dir}/")[1]
  end
  f.puts '[clean]'
  f.puts '/home/blue9057/clean_file_is_here'
end
