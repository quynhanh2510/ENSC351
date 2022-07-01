import re
import os
import sys
import pandas as pd
import collections
import fnmatch
import codecs

normal_max = 46

class grades(object):
	def __init__(self, group_path):
		self.comment_file_path = group_path + "/comments"
		self.late_time_path = group_path + "/LATE.txt"
		self.score_path = group_path + "/priority-test-score.txt"
		self.compile_comment = ""
		self.timeout_comment = ""
		self.score_pre_penalty = 0
		self.score_post_penalty = 0
		self.max_score = normal_max
		self.score_breakdown_dict = {}
		self.tests = []
		self.score_breakdown_list = []

	# detailed marking
	def getScoreBreakdown(self):
		#initialize the score dict
		for test in tests:
			self.score_breakdown_dict[test] = '0'
		# process the score file
		print(self.score_path)
		if os.path.isfile(self.score_path):
			mark = open(self.score_path, 'r')
			lines = mark.readlines()
			for line in lines:
				test_score = line.split('-')
				self.score_breakdown_dict[test_score[0]] = test_score[1].replace('\n', '')
				# self.tests.append(test_score[0])
				# self.score_breakdown_list.append(test_score[1].replace('\n', ''))
		else:
				self.score_breakdown_list = [0] * 22
		for value in self.score_breakdown_dict.values():
			self.score_pre_penalty = self.score_pre_penalty + float(value)

	def getLatePenalty(self):
		s = "That is (.+?) after the due date"
		if os.path.isfile(self.late_time_path):
			file = open(self.late_time_path)
			lines = file.readlines()
			late_time = re.search(s, lines[2]).group(1).replace(", ", "-").replace(" ", "-")
			# calculate cap reduction
			if 'day' in str(late_time):
				days = re.search("(.+?)-", late_time).group(1)
				days_late = int(days) + 1
				if (days_late > 10):
					self.max_score = 0
					self.score_pre_penalty = 0
				else:
					self.max_score = self.max_score * (1.0 - 0.05 * days_late)
			else:
				self.max_score = self.max_score * 0.9

	def applyLatePenalty(self):
		if (self.score_pre_penalty <= self.max_score):
			#keep score as is
			self.score_post_penalty = self.score_pre_penalty
		else:
			self.score_post_penalty = self.max_score;#self.max_score - (normal_max - self.score_pre_penalty)

	def getComments(self):
		# print(self.comment_file_path)
		if os.path.isfile(self.comment_file_path):
			comment_file = open(self.comment_file_path, 'r')
			lines = comment_file.readlines()
			for line in lines:
				if "program" in line:
					self.timeout_comment = "program timed out"
				if "compile" in line:
					self.compile_comment = "compilation failed"

	def populate_csv_col(self, existing):
		for key in self.score_breakdown_dict.keys():
			if key not in existing:
				existing.append(key)
		return existing


tests = ['PE(myReadcond(daSktPr[1], Ba, 200, 12, 0, 0))_1', 'strcmp(Ba,1234abcd56789)==0', 'PE_NOT(myWrite(daSktPr[1], ijkl, 5), 5)', 'PE(myWrite(daSktPr[0], tsu, 4))', 'PE(myReadcond(daSktPr[1], Ba, 200, 12, 0, 0))_2', 'strcmp(Ba,tsu)==0', 'myWrite(daSktPr[1], Ba, 200)', '(strcmp(strerror(errno),Bad file descriptor)==0)_1', 'myRead(daSktPr[1], Ba, 200)', 'myClose(daSktPr[1])_1', 'myClose(daSktPr[1])_2', '(strcmp(strerror(errno),Bad file descriptor)==0)_2', 'myClose(5000)', '(strcmp(strerror(errno),Bad file descriptor)==0)_3', 'myRead(5000, Ba, 200)', '(strcmp(strerror(errno),Bad file descriptor)==0)_4', 'myWrite(5000, Ba, 200)', '(strcmp(strerror(errno),Bad file descriptor)==0)_5', 'myTcdrain(5000)', '(strcmp(strerror(errno),Bad file descriptor)==0)_6', 'myReadcond(5000, Ba, 200, 12, 0, 0)', '(strcmp(strerror(errno),Bad file descriptor)==0)_7']

# group = "pp3/g-a3"
# curr_group = grades(group)
# curr_group.getComments()
# curr_group.getScoreBreakdown()
# print(curr_group.score_breakdown_dict.values())


d = {}
submission_dir = "pp3/"
for group in os.listdir(submission_dir):
	print(group)
	group_path = submission_dir + group
	curr_group = grades(group_path)
	# getComments() preceeds getScore()
	curr_group.getComments()
	curr_group.getScoreBreakdown()
	curr_group.getLatePenalty()
	curr_group.applyLatePenalty()
	# genereate list 
	attributes = list(curr_group.score_breakdown_dict.values()) + [curr_group.score_pre_penalty, curr_group.compile_comment + " " + curr_group.timeout_comment, curr_group.max_score, curr_group.score_post_penalty]
	# add score and comment to dict
	d[group] = attributes
	# print(d[group])
	del curr_group

# convert to csv
col = tests + ['total_score_before_penalty', 'comments', 'capped max score (if any)', 'score_after_penalty']
print(col)

df = pd.DataFrame.from_dict(data=d, orient="index", columns=col)

df_upload = pd.DataFrame.from_dict(data=d, orient="index", columns=None)
df.to_csv("marks3/part3.csv")

