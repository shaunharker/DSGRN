import pp

def helloworld():
    print 'hello world'

def parallelrun(job_server,numjobs):
    jobs=[]
    for n in range(numjobs):
        jobs.append(job_server.submit(helloworld,(),depfuncs=(),modules = (),globals=globals()))
    for job in jobs:
        job()
    job_server.print_stats()
    job_server.destroy()

if __name__ == '__main__':
    job_server = pp.Server(ppservers=("*",))
    # job_server = pp.Server(ncpus=4)
    parallelrun(job_server,100)



