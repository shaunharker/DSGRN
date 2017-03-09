import datetime, sys
def LogToSTDOUT(string=''):
  """
  Print date and time, along with optional string input, to stdout.
  """
  if string: 
    string = ':\n' + str(string)
  else:
    string = ''
  print(str(datetime.datetime.now())+string)
  sys.stdout.flush()
